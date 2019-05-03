#pragma once

#include <cstdint>
#include <variant>

#include <nlohmann/json.hpp>

#include "exceptions.h"
#include "types.h"

namespace ProcessesManagement {

/**
 *  Класс заявки на создание нового процесса.
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
  /**
   *  @brief Создает заявку на создание нового процесса.
   *
   *  @param pid Идентификатор процесса.
   *  @param ppid Идентификатор родительского процесса.
   *  @param priority Приоритет процесса.
   *  @param basePriority Базовый приоритет процесса.
   *  @param timer Время работы.
   *  @param workTime Заявленное время работы.
   *
   *  @throws ProcessesManagement::RequestException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
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

  /**
   *  Возвращает заявку в виде JSON-объекта.
   */
  nlohmann::json dump() const {
    return {{"type", "CREATE_PROCESS"},
            {"pid", _pid},
            {"ppid", _ppid},
            {"priority", _priority},
            {"basePriority", _basePriority},
            {"timer", _timer},
            {"workTime", _workTime}};
  }

  /**
   *  Возвращает дескриптор процесса, описанного в заявке.
   */
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

/**
 *  Класс заявки на завершение существующего процесса.
 */
class TerminateProcessReq {
private:
  int32_t _pid;

public:
  /**
   *  @brief Создает заявку на завершение существующего процесса.
   *
   *  @param pid Идентификатор процесса.
   *
   *  @throws ProcessesManagement::RequestException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  TerminateProcessReq(int32_t pid) : _pid(pid) {
    if (pid < 0 || pid > 255) {
      throw RequestException("INVALID_PID");
    }
  }

  int32_t pid() const { return _pid; }

  /**
   *  Возвращает заявку в виде JSON-объекта.
   */
  nlohmann::json dump() const {
    return {{"type", "TERMINATE_PROCESS"}, {"pid", _pid}};
  }
};

/**
 *  Класс заявки на инициализацию ввода/вывода.
 */
class InitIO {
private:
  int32_t _pid;

public:
  /**
   *  @brief Создает заявку на инициализацию ввода/вывода.
   *
   *  @param pid Идентификатор процесса.
   *
   *  @throws ProcessesManagement::RequestException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  InitIO(int32_t pid) : _pid(pid) {
    if (pid < 0 || pid > 255) {
      throw RequestException("INVALID_PID");
    }
  }

  int32_t pid() const { return _pid; }

  /**
   *  Возвращает заявку в виде JSON-объекта.
   */
  nlohmann::json dump() const { return {{"type", "INIT_IO"}, {"pid", _pid}}; }
};

/**
 *  Класс заявки на завершение ввода/вывода.
 */
class TerminateIO {
private:
  int32_t _pid;

public:
  /**
   *  @brief Создает заявку на завершение ввода/вывода.
   *
   *  @param pid Идентификатор процесса.
   *
   *  @throws ProcessesManagement::RequestException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  TerminateIO(int32_t pid) : _pid(pid) {
    if (pid < 0 || pid > 255) {
      throw RequestException("INVALID_PID");
    }
  }

  int32_t pid() const { return _pid; }

  /**
   *  Возвращает заявку в виде JSON-объекта.
   */
  nlohmann::json dump() const {
    return {{"type", "TERMINATE_IO"}, {"pid", _pid}};
  }
};

/**
 *  Класс заявки на передачу управления операционной системе.
 */
class TransferControl {
private:
  int32_t _pid;

public:
  /**
   *  @brief Создает заявку передачу управления операционной системе.
   *
   *  @param pid Идентификатор процесса.
   *
   *  @throws ProcessesManagement::RequestException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  TransferControl(int32_t pid) : _pid(pid) {
    if (pid < 0 || pid > 255) {
      throw RequestException("INVALID_PID");
    }
  }

  int32_t pid() const { return _pid; }

  /**
   *  Возвращает заявку в виде JSON-объекта.
   */
  nlohmann::json dump() const {
    return {{"type", "TRANSFER_CONTROL"}, {"pid", _pid}};
  }
};

/**
 *  Заявка на обработку события "Истек квант вермени".
 */
class TimeQuantumExpired {
public:
  TimeQuantumExpired() {}

  /**
   *  Возвращает заявку в виде JSON-объекта.
   */
  nlohmann::json dump() const { return {{"type", "TIME_QUANTUM_EXPIRED"}}; }
};

using Request = std::variant<CreateProcessReq, TerminateProcessReq, InitIO,
                             TerminateIO, TransferControl, TimeQuantumExpired>;
} // namespace ProcessesManagement
