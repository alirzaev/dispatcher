#pragma once

#include <cstdint>
#include <optional>
#include <set>
#include <variant>
#include <vector>

#include <algo/processes/requests.h>
#include <algo/processes/types.h>

#include "../rand_utils.h"

namespace Generators::ProcessesTask::Details {
using namespace ProcessesManagement;

template <class T> inline bool sameType(const Request &v1, const Request &v2) {
  return std::holds_alternative<T>(v1) && std::holds_alternative<T>(v2);
}
} // namespace Generators::ProcessesTask::Details

namespace Generators::ProcessesTask::TaskGenerators {
using namespace ProcessesManagement;
using std::holds_alternative;
using std::nullopt;
using std::optional;
using std::set;
using std::vector;

class AbstractTaskGenerator {
public:
  virtual ~AbstractTaskGenerator() = default;

  AbstractTaskGenerator() = default;

  virtual bool preemptive() const { return false; }

  virtual vector<Request> generate(const ProcessesState &state,
                                   optional<Request> last,
                                   bool valid = true) const {
    using Details::sameType;
    namespace PM = ProcessesManagement;

    vector<optional<Request>> requests = {
        this->CreateProcessReq(state, valid),
        this->CreateProcessReq(state, valid),
        this->TerminateProcessReq(state, valid),
        this->InitIO(state, valid),
        this->InitIO(state, valid),
        this->TerminateIO(state, valid),
        this->TerminateIO(state, valid),
        this->TransferControl(state, valid)};

    if (preemptive()) {
      requests.push_back(this->TimeQuantumExpired(state, valid));
    }

    vector<Request> filtered;
    if (!last) {
      // первая заявка - только CreateProcessReq
      for (const auto &request : requests) {
        if (request &&
            holds_alternative<PM::CreateProcessReq>(request.value())) {
          filtered.push_back(request.value());
        }
      }
    } else {
      auto lastVal = last.value();

      for (const auto &request : requests) {
        // не должно быть двух подряд идущих заявок TimeQuantumExpired или
        // TransferControl
        bool ok = request &&
                  !sameType<PM::TimeQuantumExpired>(request.value(), lastVal) &&
                  !sameType<PM::TransferControl>(request.value(), lastVal);
        if (ok) {
          filtered.push_back(request.value());
        }
      }
    }
    return filtered;
  }

  virtual optional<Request> CreateProcessReq(const ProcessesState &state,
                                             bool valid = true) const {
    auto [processes, queues] = state;
    auto usedPids = getUsedPids(state);
    auto availablePids = getAvailablePids(state);

    if (valid && !availablePids.empty()) {
      auto pid =
          RandUtils::randChoice(availablePids.begin(), availablePids.end());
      auto ppid = -1;

      // 1 из 2 заявок - дочерний процесс (если возможно)
      if (RandUtils::randRange(0, 256) % 2 == 0) {
        usedPids.insert(-1);
        auto parent = std::find_if(
            processes.begin(), processes.end(), [](const auto &process) {
              return process.state() == ProcState::EXECUTING;
            });
        ppid = parent != processes.end() ? parent->pid() : -1;

        return ProcessesManagement::CreateProcessReq(pid, ppid);
      } else {
        return ProcessesManagement::CreateProcessReq(pid, ppid);
      }
    } else if (!valid && !usedPids.empty()) {
      auto pid = RandUtils::randChoice(usedPids.begin(), usedPids.end());
      return ProcessesManagement::CreateProcessReq(pid);
    } else {
      return nullopt;
    }
  }

  virtual optional<Request> TerminateProcessReq(const ProcessesState &state,
                                                bool valid = true) const {
    auto [processes, queues] = state;
    auto usedPids = getUsedPids(state);
    auto availablePids = getAvailablePids(state);

    if (valid && !usedPids.empty()) {
      auto pid = RandUtils::randChoice(usedPids.begin(), usedPids.end());
      return ProcessesManagement::TerminateProcessReq(pid);
    } else if (!valid && !availablePids.empty()) {
      auto pid =
          RandUtils::randChoice(availablePids.begin(), availablePids.end());
      return ProcessesManagement::TerminateProcessReq(pid);
    } else {
      return nullopt;
    }
  }

  virtual optional<Request> InitIO(const ProcessesState &state,
                                   bool valid = true) const {
    auto [processes, queues] = state;
    auto usedPids = getUsedPids(state);

    auto it = std::find_if(processes.begin(), processes.end(),
                           [](const auto &process) {
                             return process.state() == ProcState::EXECUTING;
                           });
    if (it != processes.end()) {
      usedPids.erase(it->pid());
    }

    if (valid && it != processes.end()) {
      auto pid = it->pid();
      return ProcessesManagement::InitIO(pid);
    } else if (!valid && !usedPids.empty()) {
      auto pid = RandUtils::randChoice(usedPids.begin(), usedPids.end());
      return ProcessesManagement::InitIO(pid);
    } else {
      return nullopt;
    }
  }

  virtual optional<Request> TerminateIO(const ProcessesState &state,
                                        bool valid = true) const {
    auto [processes, queues] = state;
    set<int32_t> waitingPids, otherPids;
    for (const auto &process : processes) {
      if (process.state() == ProcState::WAITING) {
        waitingPids.insert(process.pid());
      }
      if (process.state() == ProcState::ACTIVE ||
          process.state() == ProcState::EXECUTING) {
        otherPids.insert(process.pid());
      }
    }

    if (valid && !waitingPids.empty()) {
      auto pid = RandUtils::randChoice(waitingPids.begin(), waitingPids.end());
      return ProcessesManagement::TerminateIO(pid);
    } else if (!valid && !otherPids.empty()) {
      auto pid = RandUtils::randChoice(otherPids.begin(), otherPids.end());
      return ProcessesManagement::TerminateIO(pid);
    } else {
      return nullopt;
    }
  }

  virtual optional<Request> TransferControl(const ProcessesState &state,
                                            bool valid = true) const {
    auto [processes, queues] = state;
    auto usedPids = getUsedPids(state);

    auto it = std::find_if(processes.begin(), processes.end(),
                           [](const auto &process) {
                             return process.state() == ProcState::EXECUTING;
                           });
    if (it != processes.end()) {
      usedPids.erase(it->pid());
    }

    if (valid && it != processes.end()) {
      auto pid = it->pid();
      return ProcessesManagement::TransferControl(pid);
    } else if (!valid && !usedPids.empty()) {
      auto pid = RandUtils::randChoice(usedPids.begin(), usedPids.end());
      return ProcessesManagement::TransferControl(pid);
    } else {
      return nullopt;
    }
  }

  virtual optional<Request> TimeQuantumExpired(const ProcessesState &,
                                               bool) const {
    return ProcessesManagement::TimeQuantumExpired();
  }

protected:
  constexpr int32_t maxPid() const { return 16; }

  set<int32_t> getAvailablePids(const ProcessesState &state) const {
    auto [processes, queues] = state;
    set<int32_t> existingPids, availabePids;
    for (const auto &process : processes) {
      existingPids.insert(process.pid());
    }
    for (int32_t pid = 0; pid < maxPid(); ++pid) {
      if (auto p = existingPids.find(pid); p == existingPids.end()) {
        availabePids.insert(pid);
      }
    }
    return availabePids;
  }

  set<int32_t> getUsedPids(const ProcessesState &state) const {
    auto [processes, queues] = state;
    set<int32_t> usedPids;
    for (const auto &process : processes) {
      usedPids.insert(process.pid());
    }
    return usedPids;
  }
};
} // namespace Generators::ProcessesTask::TaskGenerators
