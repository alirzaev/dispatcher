#pragma once

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <optional>
#include <set>
#include <utility>
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

std::optional<Process> getCurrent(const std::vector<Process> &processes) {
  auto it = std::find_if(processes.begin(), processes.end(),
                         [](const Process &process) {
                           return process.state() == ProcState::EXECUTING;
                         });
  if (it == processes.end()) {
    return std::nullopt;
  } else {
    return *it;
  }
}

template <class SequenceContainer, class Predicate>
std::remove_reference_t<SequenceContainer> filter(SequenceContainer &&container,
                                                  Predicate pred) {
  std::remove_reference_t<SequenceContainer> copy;

  std::copy_if(std::begin(container), std::end(container),
               std::back_inserter(copy), pred);

  return copy;
}
} // namespace Generators::ProcessesTask::Details

namespace Generators::ProcessesTask::TaskGenerators {
using namespace ProcessesManagement;
using std::get;
using std::get_if;
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

  virtual vector<Request>
  generate(const ProcessesState &state,
           std::pair<optional<Request>, bool> lastRequestInfo,
           bool valid = true) const {
    using Details::filter;
    using Details::sameType;
    namespace PM = ProcessesManagement;

    auto optionals = vector{this->CreateProcessReq(state, valid),
                            this->CreateProcessReq(state, valid),
                            this->CreateProcessReq(state, valid),
                            this->CreateProcessReq(state, valid),
                            this->TerminateProcessReq(state, valid),
                            this->TerminateProcessReq(state, valid),
                            this->InitIO(state, valid),
                            this->InitIO(state, valid),
                            this->InitIO(state, valid),
                            this->InitIO(state, valid),
                            this->TerminateIO(state, valid),
                            this->TerminateIO(state, valid),
                            this->TerminateIO(state, valid),
                            this->TerminateIO(state, valid),
                            this->TransferControl(state, valid),
                            this->TransferControl(state, valid)};

    if (preemptive()) {
      optionals.push_back(this->TimeQuantumExpired(state, valid));
      optionals.push_back(this->TimeQuantumExpired(state, valid));
    }

    optionals = filter(optionals, [](const auto &request) { return request; });

    vector<Request> requests;
    for (const auto &opt : optionals) {
      if (opt) {
        requests.push_back(*opt);
      }
    }

    auto last = lastRequestInfo.first;
    auto isLastValid = lastRequestInfo.second;

    if (!last) {
      // первая заявка - только CreateProcessReq
      requests = filter(requests, [](const auto &request) {
        return holds_alternative<PM::CreateProcessReq>(request);
      });
    } else if (isLastValid) {
      // не должно быть двух подряд идущих заявок TimeQuantumExpired или
      // TransferControl
      requests = filter(requests, [&var = *last](const auto &request) {
        return !sameType<PM::TimeQuantumExpired>(request, var) &&
               !sameType<PM::TransferControl>(request, var);
      });
      // после CreateProcessReq не должны идти заявки TransferControl или
      // TerminateProcessReq с таким же PID
      requests = filter(requests, [&var = *last](const auto &request) {
        auto *last = get_if<PM::CreateProcessReq>(&var);

        if (!last) {
          return true;
        }

        if (auto current = get_if<PM::TransferControl>(&request);
            current && current->pid() == last->pid()) {
          return false;
        }

        if (auto current = get_if<PM::TerminateProcessReq>(&request);
            current && current->pid() == last->pid()) {
          return false;
        }

        return true;
      });
      // после InitIO не должна идти заявка TerminateIO с таким же PID
      requests = filter(requests, [&var = *last](const auto &request) {
        auto *last = get_if<PM::InitIO>(&var);

        if (!last) {
          return true;
        }

        if (auto current = get_if<PM::TerminateIO>(&request);
            current && current->pid() == last->pid()) {
          return false;
        }

        return true;
      });
    }
    // если предыдущая заявка была заведомо некорректной, то предусловия
    // опустить
    return requests;
  }

  virtual optional<Request> CreateProcessReq(const ProcessesState &state,
                                             bool valid = true) const {
    auto [processes, queues] = state;
    auto usedPids = getUsedPids(state);
    auto availablePids = getAvailablePids(state);

    if (valid && !availablePids.empty()) {
      auto pid = RandUtils::randChoice(availablePids);
      auto ppid = -1;

      // 1 из 2 заявок - дочерний процесс (если возможно)
      if (RandUtils::randRange(0, 256) % 2 == 0) {
        usedPids.insert(-1);
        auto parent = Details::getCurrent(processes);
        ppid = parent ? parent->pid() : -1;

        return ProcessesManagement::CreateProcessReq(pid, ppid);
      } else {
        return ProcessesManagement::CreateProcessReq(pid, ppid);
      }
    } else if (!valid && !usedPids.empty()) {
      auto pid = RandUtils::randChoice(usedPids);
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
      auto pid = RandUtils::randChoice(usedPids);
      return ProcessesManagement::TerminateProcessReq(pid);
    } else if (!valid && !availablePids.empty()) {
      auto pid = RandUtils::randChoice(availablePids);
      return ProcessesManagement::TerminateProcessReq(pid);
    } else {
      return nullopt;
    }
  }

  virtual optional<Request> InitIO(const ProcessesState &state,
                                   bool valid = true) const {
    auto [processes, queues] = state;
    auto usedPids = getUsedPids(state);

    auto current = Details::getCurrent(processes);
    if (current) {
      usedPids.erase(current->pid());
    }

    if (valid && current) {
      auto pid = current->pid();
      return ProcessesManagement::InitIO(pid);
    } else if (!valid && !usedPids.empty()) {
      auto pid = RandUtils::randChoice(usedPids);
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
      auto pid = RandUtils::randChoice(waitingPids);
      return ProcessesManagement::TerminateIO(pid);
    } else if (!valid && !otherPids.empty()) {
      auto pid = RandUtils::randChoice(otherPids);
      return ProcessesManagement::TerminateIO(pid);
    } else {
      return nullopt;
    }
  }

  virtual optional<Request> TransferControl(const ProcessesState &state,
                                            bool valid = true) const {
    auto [processes, queues] = state;
    auto usedPids = getUsedPids(state);

    auto current = Details::getCurrent(processes);
    if (current) {
      usedPids.erase(current->pid());
    }

    if (valid && current) {
      auto pid = current->pid();
      return ProcessesManagement::TransferControl(pid);
    } else if (!valid && !usedPids.empty()) {
      auto pid = RandUtils::randChoice(usedPids);
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
