#pragma once

#include <algorithm>
#include <cstdint>
#include <exception>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <variant>

#include "../../../utils/overload.h"
#include "../operations.h"
#include "../requests.h"
#include "../types.h"

namespace ProcessesManagement {

enum class StrategyType { ROUNDROBIN, FCFS, SJN, SRT, WINDOWS };

/**
 * @brief Базовый класс для всех стратегий.
 */
class AbstractStrategy {
public:
  virtual ~AbstractStrategy() = default;

  AbstractStrategy() = default;

  virtual StrategyType type() const = 0;

  /**
   *  Возвращает строковое обозначение стратегии.
   */
  virtual std::string toString() const = 0;

  /**
   *  @brief Обрабатывает заявку любого типа.
   *
   *  @param request Заявка.
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Новое состояние процессов.
   */
  ProcessesState processRequest(const Request &request,
                                const ProcessesState &state) const {
    return std::visit(
        [this, state](const auto &req) {
          return updateTimer(this->processRequest(req, state));
        },
        request);
  }

  /**
   *  @brief Возвращает текстовое описание заявки.
   *
   *  @param request Заявки
   *
   *  @return Описание заявки.
   */
  virtual std::string getRequestDescription(const Request &request) const {
    using namespace std::string_literals;
    using ss = std::stringstream;

    return std::visit(
        overload{
            [](const CreateProcessReq &req) {
              if (req.ppid() == -1) {
                return static_cast<const ss &>(
                           ss() << "Возник новый процесс PID = " << req.pid())
                    .str();
              } else {
                return static_cast<const ss &>(
                           ss()
                           << "Процесс PID = " << req.ppid()
                           << " породил дочерний процесс PID = " << req.pid())
                    .str();
              }
            },
            [](const TerminateProcessReq &req) {
              return static_cast<const ss &>(ss() << "Завершен процесс PID = "
                                                  << req.pid())
                  .str();
            },
            [](const InitIO &req) {
              return static_cast<const ss &>(ss()
                                             << "Процесс PID = " << req.pid()
                                             << " выдал запрос на ввод/вывод")
                  .str();
            },
            [](const TerminateIO &req) {
              return static_cast<const ss &>(ss()
                                             << "Ввод/вывод для процесса PID = "
                                             << req.pid() << " завершен")
                  .str();
            },
            [](const TransferControl &req) {
              return static_cast<const ss &>(ss()
                                             << "Процесс PID = " << req.pid()
                                             << " передал управление "
                                                "операционной системе")
                  .str();
            },
            [](const TimeQuantumExpired &) { return "Истек квант времени"s; }},
        request);
  }

protected:
  /**
   *  @brief Обрабатывает заявку на создание нового процесса.
   *
   *  @param request Заявка.
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Новое состояние процессов.
   */
  virtual ProcessesState processRequest(const CreateProcessReq &request,
                                        const ProcessesState &state) const = 0;

  /**
   *  @brief Обрабатывает заявку завершение существующего процесса.
   *
   *  @param request Заявка.
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Новое состояние процессов.
   */
  virtual ProcessesState processRequest(const TerminateProcessReq &request,
                                        const ProcessesState &state) const = 0;

  /**
   *  @brief Обрабатывает заявку на инициализацию ввода/вывода.
   *
   *  @param request Заявка.
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Новое состояние процессов.
   */
  virtual ProcessesState processRequest(const InitIO &request,
                                        const ProcessesState &state) const = 0;

  /**
   *  @brief Обрабатывает заявку на завершение ввода/вывода.
   *
   *  @param request Заявка.
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Новое состояние процессов.
   */
  virtual ProcessesState processRequest(const TerminateIO &request,
                                        const ProcessesState &state) const = 0;

  /**
   *  @brief Обрабатывает заявку на передачу управления операционной системе.
   *
   *  @param request Заявка.
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Новое состояние процессов.
   */
  virtual ProcessesState processRequest(const TransferControl &request,
                                        const ProcessesState &state) const = 0;

  /**
   *  @brief Обрабатывает заявку "Истек квант вермени".
   *
   *  @param request Заявка.
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Новое состояние процессов.
   */
  virtual ProcessesState processRequest(const TimeQuantumExpired &request,
                                        const ProcessesState &state) const = 0;

  /**
   *  @brief Выбирает процесс, которому будет предоставлено процессорное время.
   *
   *  @param state Дескриптор состояния процессов.
   *
   *  @return Кортеж из двух чисел: PID процесса и индекс очереди, в которой он
   *  находится; либо std::nullopt, если такового нет.
   */
  virtual std::optional<std::pair<int32_t, int32_t>>
  schedule(const ProcessesState &state) const = 0;

  /**
   * @brief Возвращает дескриптор процесса, который выполняется в данный момент.
   *
   * @param state Дескриптор состояния процессов.
   *
   * @return Дескриптор процесса или std::nullopt, если такового нет.
   */
  std::optional<Process> getCurrent(const ProcessesState &state) const {
    if (auto it = getByState(state.processes, ProcState::EXECUTING);
        it != state.processes.end()) {
      return *it;
    } else {
      return std::nullopt;
    };
  }

  /**
   * @brief Возвращает дескриптор процесса по его идентификатору.
   *
   * @param state Дескриптор состояния процессов.
   * @param pid Идентификатор процесса.
   *
   * @return Дескриптор процесса или std::nullopt, если такового нет.
   */
  std::optional<Process> getProcessByPid(const ProcessesState &state,
                                         int32_t pid) const {
    if (auto it = getByPid(state.processes, pid); it != state.processes.end()) {
      return *it;
    } else {
      return std::nullopt;
    }
  }
};

using StrategyPtr = std::shared_ptr<AbstractStrategy>;
} // namespace ProcessesManagement
