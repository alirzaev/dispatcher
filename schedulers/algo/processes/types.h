#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <variant>

#include <nlohmann/json.hpp>

#include "exceptions.h"

namespace ProcessesManagement {

enum class ProcState { ACTIVE, EXECUTING, WAITING };

/**
 *  @brief Дескриптор процесса.
 */
class Process {
private:
  int32_t _pid;

  int32_t _ppid;

  size_t _priority;

  size_t _basePriority;

  int32_t _timer;

  int32_t _workTime;

  ProcState _state;

public:
  /**
   *  @brief Создает дескриптор процесса с параметрами по умолчанию.
   */
  Process()
      : _pid(0), _ppid(-1), _priority(0), _basePriority(0), _timer(0),
        _workTime(0), _state(ProcState::ACTIVE) {}

  Process(const Process &other) = default;

  Process(Process &&other) = default;

  Process &operator=(const Process &rhs) = default;

  Process &operator=(Process &&rhs) = default;

  bool operator==(const Process &rhs) const {
    return std::tuple{_pid,   _ppid,     _priority, _basePriority,
                      _timer, _workTime, _state} ==
           std::tuple{rhs._pid,          rhs._ppid,  rhs._priority,
                      rhs._basePriority, rhs._timer, rhs._workTime,
                      rhs._state};
  }

  bool operator!=(const Process &rhs) const { return !(*this == rhs); }

  bool operator<(const Process &rhs) const {
    return std::tuple{_pid,   _ppid,     _priority, _basePriority,
                      _timer, _workTime, _state} <
           std::tuple{rhs._pid,          rhs._ppid,  rhs._priority,
                      rhs._basePriority, rhs._timer, rhs._workTime,
                      rhs._state};
  }

  int32_t ppid() const { return _ppid; }

  size_t priority() const { return _priority; }

  size_t basePriority() const { return _basePriority; }

  int32_t timer() const { return _timer; }

  int32_t workTime() const { return _workTime; }

  int32_t pid() const { return _pid; }

  ProcState state() const { return _state; }

  /**
   *  @brief Изменяет идентификатор родительского процесса.
   *
   *  @param ppid Идентификатор нового родительского процесса.
   *
   *  @return Дескриптор измененного процесса.
   *
   *  @throws ProcessesManagement::TypeException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  Process ppid(int32_t ppid) const {
    if (ppid < -1 || ppid > 255) {
      throw TypeException("INVALID_PPID");
    }

    Process other = *this;
    other._ppid = ppid;

    return other;
  }

  /**
   *  @brief Изменяет приоритет процесса.
   *
   *  @param priority Новый приоритет процесса.
   *
   *  @return Дескриптор измененного процесса.
   *
   *  @throws ProcessesManagement::TypeException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  Process priority(size_t priority) const {
    if (priority > 15 || _basePriority > priority) {
      throw TypeException("INVALID_PRIORITY");
    }

    Process other = *this;
    other._priority = priority;

    return other;
  }

  /**
   *  @brief Изменяет базовый приоритет процесса.
   *
   *  @param basePriority Новый базовый приоритет процесса.
   *
   *  @return Дескриптор измененного процесса.
   *
   *  @throws ProcessesManagement::TypeException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  Process basePriority(size_t basePriority) const {
    if (basePriority > 15 || basePriority > _priority) {
      throw TypeException("INVALID_BASE_PRIORITY");
    }

    Process other = *this;
    other._basePriority = basePriority;

    return other;
  }

  /**
   *  @brief Изменяет время работы процесса.
   *
   *  @param timer Новое время работы процесса.
   *
   *  @return Дескриптор измененного процесса.
   *
   *  @throws ProcessesManagement::TypeException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  Process timer(int32_t timer) const {
    if (timer < 0) {
      throw TypeException("INVALID_TIMER");
    }

    Process other = *this;
    other._timer = timer;

    return other;
  }

  /**
   *  @brief Изменяет заявленное время работы процесса.
   *
   *  @param workTime Новое заявленное время работы процесса.
   *
   *  @return Дескриптор измененного процесса.
   *
   *  @throws ProcessesManagement::TypeException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  Process workTime(int32_t workTime) const {
    if (workTime < 0) {
      throw TypeException("INVALID_WORK_TIME");
    }

    Process other = *this;
    other._workTime = workTime;

    return other;
  }

  /**
   *  @brief Изменяет идентификатор процесса.
   *
   *  @param pid Новый идентификатор процесса.
   *
   *  @return Дескриптор измененного процесса.
   *
   *  @throws ProcessesManagement::TypeException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  Process pid(int32_t pid) const {
    if (pid < 0 || pid > 255) {
      throw TypeException("INVALID_PID");
    }

    Process other = *this;
    other._pid = pid;

    return other;
  }

  /**
   *  @brief Изменяет состояние процесса.
   *
   *  @param state Новое состояние процесса.
   *
   *  @return Дескриптор измененного процесса.
   *
   *  @throws ProcessesManagement::TypeException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  Process state(ProcState state) const {
    Process other = *this;
    other._state = state;

    return other;
  }

  /**
   *  Возвращает дескриптор в виде JSON-объекта.
   */
  nlohmann::json dump() const {
    std::string state;
    switch (_state) {
    case ProcState::ACTIVE:
      state = "ACTIVE";
      break;
    case ProcState::EXECUTING:
      state = "EXECUTING";
      break;
    case ProcState::WAITING:
      state = "WAITING";
      break;
    }
    return {{"pid", _pid},           {"ppid", _ppid},
            {"priority", _priority}, {"basePriority", _basePriority},
            {"timer", _timer},       {"workTime", _workTime},
            {"state", state}};
  }
};

/**
 *  @brief Дескриптор состояния процессов.
 *
 *  Процессам в данной модели могут присваиваться идентификаторы (PID) от 0 до
 *  255 включительно.
 *
 *  Программная модель разработана исходя из того, что перед обработкой первой
 *  заявки очереди пусты и ни один процесс не создан.
 */
class ProcessesState {
public:
  std::vector<Process> processes;

  std::array<std::deque<int32_t>, 16> queues;

  /**
   *  @brief Создает дескриптор состояния процессов с заданными параметрами.
   *
   *  @param processes Список дескрипторов процессов.
   *  @param queues Список очередй.
   */
  ProcessesState(const std::vector<Process> &processes,
                 const std::array<std::deque<int32_t>, 16> &queues)
      : processes(processes), queues(queues) {}

  /**
   *  @brief Создает дескриптор состояния процессов с заданными параметрами.
   *  @param processes Список дескрипторов процессов.
   *  @param queues Отображение вида <индекс очереди> -> <список PID'ов>.
   */
  ProcessesState(const std::vector<Process> &processes,
                 const std::map<size_t, std::deque<int32_t>> &queues)
      : processes(processes), queues() {
    for (const auto &queue : queues) {
      this->queues.at(queue.first) = queue.second;
    }
  }

  ProcessesState() : ProcessesState(ProcessesState::initial()) {}

  ProcessesState(const ProcessesState &state) = default;

  ProcessesState(ProcessesState &&state) = default;

  ProcessesState &operator=(const ProcessesState &state) = default;

  ProcessesState &operator=(ProcessesState &&state) = default;

  bool operator==(const ProcessesState &state) const {
    return processes == state.processes && queues == state.queues;
  }

  bool operator!=(const ProcessesState &state) const {
    return !(*this == state);
  }

  /**
   *  Возвращает дескриптор в виде JSON-объекта.
   */
  nlohmann::json dump() const {
    auto jsonProcesses = nlohmann::json::array();
    auto jsonQueues = nlohmann::json::array();

    for (const auto &process : processes) {
      jsonProcesses.push_back(process.dump());
    }
    for (const auto &queue : queues) {
      jsonQueues.push_back(queue);
    }

    return {{"processes", jsonProcesses}, {"queues", jsonQueues}};
  }

  /**
   *  Возвращает дескриптор с начальным состоянием.
   */
  static ProcessesState initial() {
    return {{}, std::array<std::deque<int32_t>, 16>{}};
  }

  /**
   *  @brief Проверяет параметры конструктора.
   *
   *  @param processes Список дескрипторов процессов.
   *  @param queues Список очередй.
   *
   *  @throws ProcessesManagement::TypeException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  static void validate(const std::vector<Process> &processes,
                       const std::array<std::deque<int32_t>, 16> &queues) {
    std::set<int32_t> pidsInQueues, pidsOfProcesses, activePids;
    std::map<size_t, std::set<int32_t>> queuesMap;

    // В списке процессов не должно быть двух процессов с одинаковым PID.
    for (const auto &process : processes) {
      if (pidsOfProcesses.find(process.pid()) != pidsOfProcesses.end()) {
        throw TypeException("INVALID_STATE");
      } else {
        pidsOfProcesses.insert(process.pid());
      }
    }

    // Собираем список процессов в состоянии ACTIVE.
    for (const auto &process : processes) {
      if (process.state() == ProcState::ACTIVE) {
        activePids.insert(process.pid());
      }
    }

    // Проверяем на существование родительских процессов.
    for (const auto &process : processes) {
      if (process.ppid() != -1 &&
          pidsOfProcesses.find(process.ppid()) == pidsOfProcesses.end()) {
        throw TypeException("INVALID_STATE");
      }
    }

    // Собираем множество процессов, находящихся в очередях.
    for (size_t i = 0; i < queues.size(); ++i) {
      const auto &queue = queues[i];
      for (const auto &pid : queue) {
        // Каждый процесс должен находится только в одной очереди.
        if (pidsInQueues.find(pid) != pidsInQueues.end()) {
          throw TypeException("INVALID_STATE");
        } else {
          pidsInQueues.insert(pid);
        }
        queuesMap[i].insert(pid);
      }
    }

    // Только процессы с состоянием ACTIVE могут находится в очередях.
    if (activePids != pidsInQueues) {
      throw TypeException("INVALID_STATE");
    }

    // Проверяем процессы на соответствие приоритета и индекса очереди.
    for (const auto &process : processes) {
      if (process.state() != ProcState::ACTIVE) {
        continue;
      }
      auto priority = process.priority();
      const auto &queueSet = queuesMap[priority];
      if (queueSet.find(process.pid()) == queueSet.end()) {
        throw TypeException("INVALID_STATE");
      }
    }
  }
};
} // namespace ProcessesManagement
