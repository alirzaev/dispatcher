#pragma once

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <set>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <mapbox/variant.hpp>
#include <tl/optional.hpp>

#include <algo/processes/helpers.h>
#include <algo/processes/requests.h>
#include <algo/processes/strategies.h>
#include <algo/processes/types.h>

#include "../rand_utils.h"

namespace Generators::ProcessesTask::Details {
using namespace ProcessesManagement;

template <class T>
inline bool sameType(const Request &v1, const Request &v2) {
  return v1.is<T>() && v2.is<T>();
}

template <class SequenceContainer, class Predicate>
std::remove_reference_t<SequenceContainer> filter(SequenceContainer &&container,
                                                  Predicate pred) {
  std::remove_reference_t<SequenceContainer> copy;

  std::copy_if(std::begin(container),
               std::end(container),
               std::back_inserter(copy),
               pred);

  return copy;
}

template <class T1, class T2>
bool equalsByPid(const Request &v1, const Request &v2) {
  if ((v1.template is<TimeQuantumExpired>() &&
       !v2.template is<TimeQuantumExpired>()) ||
      (!v1.template is<TimeQuantumExpired>() &&
       v2.template is<TimeQuantumExpired>())) {
    return false;
  }
  if (sameType<TimeQuantumExpired>(v1, v2)) {
    return true;
  }
  if (v1.template is<T1>() && v2.template is<T2>()) {
    return v1.get<T1>().pid() == v2.get<T2>().pid();
  } else {
    return false;
  }
}
} // namespace Generators::ProcessesTask::Details

namespace Generators::ProcessesTask::TaskGenerators {
using namespace ProcessesManagement;
using std::set;
using std::vector;
using tl::nullopt;
using tl::optional;

class AbstractTaskGenerator {
private:
  vector<optional<Request>> basis(const ProcessesState &state,
                                  bool valid = true) const {
    auto [active, waiting, executing] = collectStats(state);

    auto optionals = vector{this->CreateProcessReq(state, valid),
                            this->CreateProcessReq(state, valid),
                            this->CreateProcessReq(state, valid),
                            this->TerminateProcessReq(state, valid),
                            this->TerminateProcessReq(state, valid),
                            this->TerminateProcessReq(state, valid),
                            this->InitIO(state, valid),
                            this->TerminateIO(state, valid),
                            this->TransferControl(state, valid),
                            this->TransferControl(state, valid)};

    if (active < 5 || executing < 1) {
      optionals.push_back(this->CreateProcessReq(state, valid));
      optionals.push_back(this->CreateProcessReq(state, valid));
    }

    if (active > 7) {
      optionals.push_back(this->TerminateProcessReq(state, valid));
      optionals.push_back(this->TerminateProcessReq(state, valid));
      optionals.push_back(this->TerminateProcessReq(state, valid));
    }

    if (waiting < 1) {
      optionals.push_back(this->InitIO(state, valid));
      optionals.push_back(this->InitIO(state, valid));
    }

    if (waiting > 3) {
      optionals.push_back(this->TerminateIO(state, valid));
      optionals.push_back(this->TerminateIO(state, valid));
    }

    if (preemptive()) {
      optionals.push_back(this->TimeQuantumExpired(state, valid));
      optionals.push_back(this->TimeQuantumExpired(state, valid));
    } else {
      optionals.push_back(this->TransferControl(state, valid));
      optionals.push_back(this->TransferControl(state, valid));
    }

    return optionals;
  }

  vector<Request> applyFilters(vector<optional<Request>> optionals,
                               optional<Request> last,
                               bool valid) const {
    using Details::equalsByPid;
    using Details::filter;
    using Details::sameType;
    namespace PM = ProcessesManagement;

    vector<Request> requests;
    for (const auto &opt : optionals) {
      if (opt) {
        requests.push_back(*opt);
      }
    }
    requests = filter(requests, [last, valid](const auto &req) {
      // если предыдущая заявка была заведомо некорректной, то предусловия
      // опустить
      if (!valid) {
        return true;
      }
      // первая заявка - только CreateProcessReq
      if (!last) {
        return req.template is<PM::CreateProcessReq>();
      }

      bool ok = true;
      // не должно быть двух подряд идущих заявок TimeQuantumExpired или
      // TransferControl
      ok &= !sameType<PM::TimeQuantumExpired>(*last, req);
      ok &= !sameType<PM::TransferControl>(*last, req);

      // после CreateProcessReq не должны идти заявки TransferControl или
      // TerminateProcessReq с таким же PID
      ok &= !equalsByPid<PM::CreateProcessReq, PM::TransferControl>(*last, req);
      ok &= !equalsByPid<PM::CreateProcessReq, PM::TerminateProcessReq>(*last,
                                                                        req);

      // после InitIO не должна идти заявка TerminateIO с таким же PID
      ok &= !equalsByPid<PM::InitIO, PM::TerminateIO>(*last, req);

      return ok;
    });

    return requests;
  }

public:
  struct Stats {
    int active;
    int waiting;
    int executing;
  };

  virtual ~AbstractTaskGenerator() = default;

  AbstractTaskGenerator() = default;

  virtual bool preemptive() const { return false; }

  virtual vector<Request>
  generate(const ProcessesState &state,
           std::pair<optional<Request>, bool> lastRequestInfo,
           bool valid = true) const {
    auto [last, lastValid] = lastRequestInfo;

    auto optionals = basis(state, valid);

    auto requests = applyFilters(optionals, last, lastValid);

    return requests;
  }

  Stats collectStats(const ProcessesState &state) const {
    int active = 0, waiting = 0, executing = 0;
    for (const auto &process : state.processes) {
      if (process.state() == ProcState::ACTIVE) {
        active++;
      } else if (process.state() == ProcState::WAITING) {
        waiting++;
      } else {
        executing++;
      }
    }

    return {active, waiting, executing};
  }

  std::pair<ProcessesState, vector<Request>> bootstrap(ProcessesState state,
                                                       StrategyPtr strategy) {
    using Ptr = std::function<optional<Request>(const ProcessesState &)>;
    vector<Ptr> genFns{
        [this](const ProcessesState &state) {
          return this->CreateProcessReq(state);
        },
        [this](const ProcessesState &state) {
          return this->CreateProcessReq(state);
        },
        [this](const ProcessesState &state) {
          return this->TimeQuantumExpired(state);
        },
        [this](const ProcessesState &state) {
          return this->CreateProcessReq(state);
        },
        [this](const ProcessesState &state) {
          return this->CreateProcessReq(state);
        },
        [this](const ProcessesState &state) {
          return this->TransferControl(state);
        },
        [this](const ProcessesState &state) {
          return this->CreateProcessReq(state);
        },
        [this](const ProcessesState &state) { return this->InitIO(state); }};

    auto rand = RandUtils::randRange(0, 255);
    if (rand % 3 == 0) {
      // TimeQuantumExpired <-> TransferControl
      std::swap(genFns[2], genFns[5]);
      if (!preemptive()) {
        genFns.erase(genFns.begin() + 5);
      }
    } else if (rand % 3 == 1) {
      // TransferControl <-> InitIO
      std::swap(genFns[5], genFns[7]);
      if (!preemptive()) {
        genFns.erase(genFns.begin() + 2);
      }
    } else {
      if (!preemptive()) {
        genFns.erase(genFns.begin() + 2);
      }
    }

    vector<Request> requests;
    for (const auto &genFn : genFns) {
      if (auto opt = genFn(state); opt.has_value()) {
        requests.push_back(*opt);
        state = strategy->processRequest(*opt, state);
      }
    }

    return {state, requests};
  }

  virtual optional<Request> CreateProcessReq(const ProcessesState &state,
                                             bool valid = true) const {
    auto [processes, queues] = state;
    auto usedPids = getUsedPids(state);
    auto availablePids = getAvailablePids(state);

    if (valid && !availablePids.empty()) {
      auto pid = RandUtils::randChoice(availablePids);
      auto ppid = -1;

      // 3 из 5 заявок - дочерний процесс (если возможно)
      if (RandUtils::randRange(0, 256) % 5 < 2) {
        return ProcessesManagement::CreateProcessReq(pid, ppid);
      } else {
        usedPids.insert(-1);
        auto parentIndex = getIndexByState(processes, ProcState::EXECUTING);
        ppid = parentIndex ? processes.at(*parentIndex).pid() : -1;

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

    auto currentIndex = getIndexByState(processes, ProcState::EXECUTING);
    if (currentIndex) {
      usedPids.erase(processes.at(*currentIndex).pid());
    }

    if (valid && currentIndex &&
        !usedPids.empty()) { // хотя бы один процесс должен исполняться на
                             // процессоре
      auto pid = processes.at(*currentIndex).pid();
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

    auto currentIndex = getIndexByState(processes, ProcState::EXECUTING);
    if (currentIndex) {
      usedPids.erase(processes.at(*currentIndex).pid());
    }

    if (valid && currentIndex) {
      auto pid = processes.at(*currentIndex).pid();
      return ProcessesManagement::TransferControl(pid);
    } else if (!valid && !usedPids.empty()) {
      auto pid = RandUtils::randChoice(usedPids);
      return ProcessesManagement::TransferControl(pid);
    } else {
      return nullopt;
    }
  }

  virtual optional<Request> TimeQuantumExpired(const ProcessesState &,
                                               bool = true) const {
    return ProcessesManagement::TimeQuantumExpired();
  }

protected:
  constexpr int32_t maxPid() const { return 24; }

  set<int32_t> getAvailablePids(const ProcessesState &state) const {
    set<int32_t> existingPids, availabePids;
    for (const auto &process : state.processes) {
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
    set<int32_t> usedPids;
    for (const auto &process : state.processes) {
      usedPids.insert(process.pid());
    }
    return usedPids;
  }
};
} // namespace Generators::ProcessesTask::TaskGenerators
