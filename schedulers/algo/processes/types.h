#pragma once

#include "exceptions.h"
#include <cstdint>
#include <string>
#include <tuple>
#include <variant>

#include <nlohmann/json.hpp>

namespace ProcessesManagement {

enum class ProcessState { ACTIVE, EXECUTING, WAITING };

class Process {
private:
  int32_t _pid;

  int32_t _ppid;

  int32_t _priority;

  int32_t _basePriority;

  int32_t _timer;

  int32_t _workTime;

  ProcessState _state;

public:
  Process()
      : _pid(0), _ppid(-1), _priority(0), _basePriority(0), _timer(0),
        _workTime(0), _state(ProcessState::ACTIVE) {}

  Process(const Process &other) = default;

  Process &operator=(const Process &rhs) = default;

  bool operator==(const Process &rhs) const {
    return std::tuple{_pid,   _ppid,     _priority, _basePriority,
                      _timer, _workTime, _state} ==
           std::tuple{rhs._pid,          rhs._ppid,  rhs._priority,
                      rhs._basePriority, rhs._timer, rhs._workTime,
                      rhs._state};
  }

  int32_t ppid() const { return _ppid; }

  int32_t priority() const { return _priority; }

  int32_t basePriority() const { return _basePriority; }

  int32_t timer() const { return _timer; }

  int32_t workTime() const { return _workTime; }

  int32_t pid() const { return _pid; }

  ProcessState state() const { return _state; }

  Process ppid(int32_t ppid) const {
    if (ppid < -1 || ppid > 255) {
      throw TypeException("INVALID_PPID");
    }

    Process other = *this;
    other._ppid = ppid;

    return other;
  }

  Process priority(int32_t priority) const {
    if (priority < 0 || priority > 15) {
      throw TypeException("INVALID_PRIORITY");
    }

    Process other = *this;
    other._priority = priority;

    return other;
  }

  Process basePriority(int32_t basePriority) const {
    if (basePriority < 0 || basePriority > 15 || basePriority > _priority) {
      throw TypeException("INVALID_BASE_PRIORITY");
    }

    Process other = *this;
    other._basePriority = basePriority;

    return other;
  }

  Process timer(int32_t timer) const {
    if (timer < 0) {
      throw TypeException("INVALID_TIMER");
    }

    Process other = *this;
    other._timer = timer;

    return other;
  }

  Process workTime(int32_t workTime) const {
    if (workTime < 0) {
      throw TypeException("INVALID_WORK_TIME");
    }

    Process other = *this;
    other._workTime = workTime;

    return other;
  }

  Process pid(int32_t pid) const {
    if (pid < 0 || pid > 255) {
      throw TypeException("INVALID_PID");
    }

    Process other = *this;
    other._pid = pid;

    return other;
  }

  Process state(ProcessState state) const {
    Process other = *this;
    other._state = state;

    return other;
  }

  nlohmann::json dump() const {
    std::string state;
    switch (_state) {
    case ProcessState::ACTIVE:
      state = "ACTIVE";
      break;
    case ProcessState::EXECUTING:
      state = "EXECUTING";
      break;
    case ProcessState::WAITING:
      state = "WAITING";
      break;
    }
    return {{"pid", _pid},           {"ppid", _ppid},
            {"priority", _priority}, {"basePriority", _basePriority},
            {"timer", _timer},       {"workTime", _workTime},
            {"state", state}};
  }
};
} // namespace ProcessesManagement
