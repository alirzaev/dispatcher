#pragma once

#include "exceptions.h"
#include "types.h"

#include <cstdint>
#include <variant>

#include <nlohmann/json.hpp>

namespace ProcessesManagement {

/*
 * Класс заявки на создание нового процесса
 */
class CreateProcessReq {
private:
  int32_t _pid;

  int32_t _ppid;

  int32_t _priority;

  int32_t _basePriority;

  int32_t _timer;

  int32_t _workTime;

public:
  /*
   * Заявка на создание нового процесса
   *
   * @param pid идентификатор процесса
   * @param ppid идентификатор родительского процесса
   * @param priority текущий приоритет
   * @param basePriority базовый приоритет
   * @param timer время работы
   * @param workTime заявленное время работы
   *
   */
  CreateProcessReq(int32_t pid, int32_t ppid = -1, int32_t priority = 0,
                   int32_t basePriority = 0, int32_t timer = 0,
                   int32_t workTime = 0)
      : _pid(pid), _ppid(ppid), _priority(priority),
        _basePriority(basePriority), _timer(timer), _workTime(workTime) {
    if (pid < 0 || pid > 255) {
      throw RequestException("INVALID_PID");
    }
    if (ppid < -1 || ppid > 255) {
      throw RequestException("INVALID_PPID");
    }
    if (priority < 0 || priority > 15) {
      throw RequestException("INVALID_PRIORITY");
    }
    if (basePriority < 0 || basePriority > 15 || basePriority > priority) {
      throw RequestException("INVALID_BASE_PRIORITY");
    }
    if (timer < 0) {
      throw RequestException("INVALID_TIMER");
    }
    if (workTime < 0) {
      throw RequestException("INVALID_WORK_TIME");
    }
  }

  int32_t ppid() const { return _ppid; }

  int32_t priority() const { return _priority; }

  int32_t basePriority() const { return _basePriority; }

  int32_t timer() const { return _timer; }

  int32_t workTime() const { return _workTime; }

  int32_t pid() const { return _pid; }

  nlohmann::json dump() const {
    return {{"type", "CREATE_PROCESS"},
            {"pid", _pid},
            {"ppid", _ppid},
            {"priority", _priority},
            {"basePriority", _basePriority},
            {"timer", _timer},
            {"workTime", _workTime}};
  }

  Process toProcess() const {
    return Process{}
        .pid(pid())
        .ppid(ppid())
        .priority(priority())
        .basePriority(basePriority())
        .timer(timer())
        .workTime(workTime());
  }
};

/*
 * Класс заявки на завершение существующего процесса
 */
class TerminateProcessReq {
private:
  int32_t _pid;

public:
  /*
   * Заявка на завершение существующего процесса
   *
   * @param pid идентификатор процесса
   *
   */
  TerminateProcessReq(int32_t pid) : _pid(pid) {
    if (pid < 0 || pid > 255) {
      throw RequestException("INVALID_PID");
    }
  }

  int32_t pid() const { return _pid; }

  nlohmann::json dump() const {
    return {{"type", "TERMINATE_PROCESS"}, {"pid", _pid}};
  }
};

/*
 * Класс заявки на инициализацию ввода/вывода
 */
class InitIO {
private:
  int32_t _pid;

public:
  /*
   * Заявка на инициализацию ввода/вывода
   *
   * @param pid идентификатор процесса
   *
   */
  InitIO(int32_t pid) : _pid(pid) {
    if (pid < 0 || pid > 255) {
      throw RequestException("INVALID_PID");
    }
  }

  int32_t pid() const { return _pid; }

  nlohmann::json dump() const { return {{"type", "INIT_IO"}, {"pid", _pid}}; }
};

/*
 * Класс заявки на завершение ввода/вывода
 */
class TerminateIO {
private:
  int32_t _pid;

public:
  /*
   * Заявка на завершение ввода/вывода
   *
   * @param pid идентификатор процесса
   *
   */
  TerminateIO(int32_t pid) : _pid(pid) {
    if (pid < 0 || pid > 255) {
      throw RequestException("INVALID_PID");
    }
  }

  int32_t pid() const { return _pid; }

  nlohmann::json dump() const {
    return {{"type", "TERMINATE_IO"}, {"pid", _pid}};
  }
};

/*
 * Класс заявки на передачу управления операционной системе
 */
class TransferControl {
private:
  int32_t _pid;

public:
  /*
   * Заявка на завершение ввода/вывода
   *
   * @param pid идентификатор процесса
   *
   */
  TransferControl(int32_t pid) : _pid(pid) {
    if (pid < 0 || pid > 255) {
      throw RequestException("INVALID_PID");
    }
  }

  int32_t pid() const { return _pid; }

  nlohmann::json dump() const {
    return {{"type", "TRANSFER_CONTROL"}, {"pid", _pid}};
  }
};

/*
 * Истек квант вермени
 */
class TimeQuantumExpired {
public:
  TimeQuantumExpired() {}

  nlohmann::json dump() const { return {{"type", "TIME_QUANTUM_EXPIRED"}}; }
};

using Request = std::variant<CreateProcessReq, TerminateProcessReq, InitIO,
                             TerminateIO, TransferControl, TimeQuantumExpired>;
} // namespace ProcessesManagement
