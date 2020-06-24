#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <mapbox/variant.hpp>
#include <nlohmann/json.hpp>
#include <tl/optional.hpp>

#include "../algo/memory/exceptions.h"
#include "../algo/memory/requests.h"
#include "../algo/memory/strategies.h"
#include "../algo/processes/exceptions.h"
#include "../algo/processes/requests.h"
#include "../algo/processes/strategies.h"
#include "exceptions.h"

namespace Utils {
namespace Memory = MemoryManagement;
namespace Processes = ProcessesManagement;

/**
 *  @brief Задание "Диспетчеризация памяти".
 */
class MemoryTask {
private:
  Memory::StrategyPtr _strategy;

  uint32_t _completed;

  Memory::MemoryState _state;

  std::vector<Memory::Request> _requests;

  uint32_t _fails;

  std::vector<std::string> _actions;

  /**
   *  @brief Создает объект задания "Диспетчеризация памяти".
   *
   *  @param strategy Стратегия выбора блока памяти.
   *  @param completed Количество обработанных заявок.
   *  @param fails Количество допущенных пользователем ошибок.
   *  @param state Дескриптор состояния памяти.
   *  @param requests Список заявок для обработки.
   *  @param actions Массив строк с информацией о действиях пользователя для
   *  каждой заявки.
   */
  MemoryTask(Memory::StrategyPtr strategy,
             uint32_t completed,
             uint32_t fails,
             const Memory::MemoryState &state,
             const std::vector<Memory::Request> &requests,
             const std::vector<std::string> &actions)
      : _strategy(strategy), _completed(completed), _state(state),
        _requests(requests), _fails(fails), _actions(actions) {}

public:
  /**
   *  @brief Создает объект задания "Диспетчеризация памяти".
   *
   *  @see Utils::MemoryTask::MemoryTask().
   */
  static MemoryTask create(Memory::StrategyPtr strategy,
                           uint32_t completed,
                           uint32_t fails,
                           const Memory::MemoryState &state,
                           const std::vector<Memory::Request> &requests,
                           const std::vector<std::string> &actions) {
    validate(strategy, completed, state, requests);
    return {strategy, completed, fails, state, requests, actions};
  }

  /**
   *  @brief Создает объект задания "Диспетчеризация памяти".
   *
   *  Количество допущенных пользователем ошибок по умолчанию - 0.
   *  Массив действий actions пользователя по умолчанию пуст.
   *
   *  @see Utils::MemoryTask::MemoryTask().
   */
  static MemoryTask create(Memory::StrategyPtr strategy,
                           uint32_t completed,
                           const Memory::MemoryState &state,
                           const std::vector<Memory::Request> &requests) {
    validate(strategy, completed, state, requests);
    return {strategy, completed, 0, state, requests, {}};
  }

  /**
   *  @brief Проверяет параметры конструктора.
   *
   *  @param strategy Стратегия выбора блока памяти.
   *  @param completed Количество обработанных заявок.
   *  @param state Дескриптор состояния памяти.
   *  @param requests Список заявок для обработки.
   *
   *  @throws Utils::TaskException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  static void validate(Memory::StrategyPtr strategy,
                       uint32_t completed,
                       const Memory::MemoryState &state,
                       const std::vector<Memory::Request> &requests) {
    try {
      Memory::MemoryState::validate(state.blocks, state.freeBlocks);
    } catch (Memory::BaseException &ex) {
      throw TaskException(ex.what());
    }

    if (requests.size() < completed) {
      throw TaskException("INVALID_TASK");
    }
    auto currentState = Memory::MemoryState::initial();
    try {
      for (auto req = requests.begin(); req != requests.begin() + completed;
           ++req) {
        currentState = strategy->processRequest(*req, currentState);
      }
      if (currentState != state) {
        throw TaskException("STATE_MISMATCH");
      }
    } catch (Memory::BaseException &ex) {
      throw TaskException(ex.what());
    }
  }

  Memory::StrategyPtr strategy() const { return _strategy; }

  uint32_t completed() const { return _completed; }

  const Memory::MemoryState &state() const { return _state; }

  const std::vector<Memory::Request> &requests() const { return _requests; }

  uint32_t fails() const { return _fails; }

  const std::vector<std::string> actions() const { return _actions; }

  /**
   *  Возвращает задание в виде JSON-объекта.
   */
  nlohmann::json dump() const {
    nlohmann::json obj;

    obj["type"] = "MEMORY_TASK";

    obj["strategy"] = strategy()->toString();

    obj["completed"] = completed();

    obj["state"] = state().dump();

    obj["fails"] = fails();

    obj["requests"] = nlohmann::json::array();

    for (auto request : requests()) {
      auto req_json = request.match([](const auto &req) { return req.dump(); });
      obj["requests"].push_back(req_json);
    }

    obj["actions"] = nlohmann::json(_actions);

    return obj;
  }

  /**
   *  Проверяет, выполнено ли задание полностью.
   */
  bool done() const { return _completed == _requests.size(); }

  /**
   *  @brief Проверяет, правильно ли обработана текущая заявка.
   *
   *  @param state Дескриптор состояния памяти после обработки заявки.
   *
   *  @return Пара <bool ok, MemoryTask task>.
   *
   *  Если заявка была обработана правильно, то @a ok == true, а в @a task
   *  хранится новый объект задания с обновленными состоянием памяти и
   *  счетчиком завершенных заданий (увеличивается на 1). Если задание выполнено
   *  полностью, то в @a task будет хранится текущий объект задания.
   *
   *  Если заявка была обработана неправильно, то @a ok == false, а в @a task
   *  хранится текущий объект задания с увеличенным на единицу счетчиком ошибок.
   */
  std::pair<bool, MemoryTask> next(const Memory::MemoryState &state) const {
    if (done()) {
      return {true, *this};
    }
    try {
      auto request = _requests[_completed];
      auto expected = _strategy->processRequest(request, _state);
      if (expected == state) {
        return {true,
                MemoryTask{_strategy,
                           _completed + 1,
                           _fails,
                           expected,
                           _requests,
                           _actions}};
      } else {
        return {false,
                MemoryTask{_strategy,
                           _completed,
                           _fails + 1,
                           _state,
                           _requests,
                           _actions}};
      }
    } catch (...) {
      return {
          false,
          MemoryTask{
              _strategy, _completed, _fails + 1, _state, _requests, _actions}};
    }
  }
};

/**
 *  @brief Задание "Диспетчеризация процессов".
 */
class ProcessesTask {
private:
  Processes::StrategyPtr _strategy;

  uint32_t _completed;

  Processes::ProcessesState _state;

  std::vector<Processes::Request> _requests;

  uint32_t _fails;

  std::vector<std::string> _actions;

  /**
   *  @brief Создает объект задания "Диспетчеризация процессов".
   *
   *  @param strategy Планировщик.
   *  @param completed Количество обработанных заявок.
   *  @param fails Количество допущенных пользователем ошибок.
   *  @param state Дескриптор состояния процессов.
   *  @param requests Список заявок для обработки.
   *  @param actions Массив строк с информацией о действиях пользователя для
   *  каждой заявки.
   */
  ProcessesTask(Processes::StrategyPtr strategy,
                uint32_t completed,
                uint32_t fails,
                const Processes::ProcessesState &state,
                const std::vector<Processes::Request> &requests,
                const std::vector<std::string> &actions)
      : _strategy(strategy), _completed(completed), _state(state),
        _requests(requests), _fails(fails), _actions(actions) {}

public:
  /**
   *  @brief Создает объект задания "Диспетчеризация процессов".
   *
   *  @see Utils::ProcessesTask::ProcessesTask().
   */
  static ProcessesTask create(Processes::StrategyPtr strategy,
                              uint32_t completed,
                              uint32_t fails,
                              const Processes::ProcessesState &state,
                              const std::vector<Processes::Request> &requests,
                              const std::vector<std::string> &actions) {
    validate(strategy, completed, state, requests);
    return {strategy, completed, fails, state, requests, actions};
  }

  /**
   *  @brief Создает объект задания "Диспетчеризация процессов".
   *
   *  Количество допущенных пользователем ошибок по умолчанию - 0.
   *  Массив действий actions пользователя по умолчанию пуст.
   *
   *  @see Utils::ProcessesTask::ProcessesTask().
   */
  static ProcessesTask create(Processes::StrategyPtr strategy,
                              uint32_t completed,
                              const Processes::ProcessesState &state,
                              const std::vector<Processes::Request> &requests) {
    validate(strategy, completed, state, requests);
    return {strategy, completed, 0, state, requests, {}};
  }

  /**
   *  @brief Проверяет параметры конструктора.
   *
   *  @param strategy Стратегия выбора блока памяти.
   *  @param completed Количество обработанных заявок.
   *  @param state Дескриптор состояния памяти.
   *  @param requests Список заявок для обработки.
   *
   *  @throws Utils::TaskException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  static void validate(Processes::StrategyPtr strategy,
                       uint32_t completed,
                       const Processes::ProcessesState &state,
                       const std::vector<Processes::Request> &requests) {
    try {
      Processes::ProcessesState::validate(state.processes, state.queues);
    } catch (Processes::BaseException &ex) {
      throw TaskException(ex.what());
    }

    if (requests.size() < completed) {
      throw TaskException("INVALID_TASK");
    }
    auto currentState = Processes::ProcessesState::initial();
    try {
      for (auto req = requests.begin(); req != requests.begin() + completed;
           ++req) {
        currentState = strategy->processRequest(*req, currentState);
      }
      if (currentState != state) {
        throw TaskException("STATE_MISMATCH");
      }
    } catch (Memory::BaseException &ex) {
      throw TaskException(ex.what());
    }
  }

  /**
   *  Возвращает задание в виде JSON-объекта.
   */
  nlohmann::json dump() const {
    nlohmann::json obj;

    obj["type"] = "PROCESSES_TASK";

    obj["strategy"] = strategy()->toString();

    obj["completed"] = completed();

    obj["state"] = state().dump();

    obj["fails"] = fails();

    obj["requests"] = nlohmann::json::array();

    for (auto request : requests()) {
      auto req_json = request.match([](const auto &req) { return req.dump(); });
      obj["requests"].push_back(req_json);
    }

    obj["actions"] = nlohmann::json(_actions);

    return obj;
  }

  Processes::StrategyPtr strategy() const { return _strategy; }

  uint32_t completed() const { return _completed; }

  const Processes::ProcessesState &state() const { return _state; }

  const std::vector<Processes::Request> &requests() const { return _requests; }

  uint32_t fails() const { return _fails; }

  const std::vector<std::string> actions() const { return _actions; }

  /**
   *  Проверяет, выполнено ли задание полностью.
   */
  bool done() const { return _completed == _requests.size(); }

  /**
   *  @brief Проверяет, правильно ли обработана текущая заявка.
   *
   *  @param state Дескриптор состояния процессов после обработки заявки.
   *
   *  @return Пара <bool ok, ProcessesTask task>.
   *
   *  Если заявка была обработана правильно, то @a ok == true, а в @a task
   *  хранится новый объект задания с обновленными состоянием процессов и
   *  счетчиком завершенных заданий (увеличивается на 1). Если задание выполнено
   *  полностью, то в @a task будет хранится текущий объект задания.
   *
   *  Если заявка была обработана неправильно, то @a ok == false, а в @a task
   *  хранится текущий объект задания с увеличенным на единицу счетчиком ошибок.
   */
  std::pair<bool, ProcessesTask>
  next(const Processes::ProcessesState &state) const {
    if (done()) {
      return {true, *this};
    }
    try {
      auto request = _requests[_completed];
      auto expected = _strategy->processRequest(request, _state);
      if (expected == state) {
        return {true,
                ProcessesTask{_strategy,
                              _completed + 1,
                              _fails,
                              expected,
                              _requests,
                              _actions}};
      } else {
        return {false,
                ProcessesTask{_strategy,
                              _completed,
                              _fails + 1,
                              _state,
                              _requests,
                              _actions}};
      }
    } catch (...) {
      return {
          false,
          ProcessesTask{
              _strategy, _completed, _fails + 1, _state, _requests, _actions}};
    }
  }
};

using Task = mapbox::util::variant<MemoryTask, ProcessesTask>;
} // namespace Utils
