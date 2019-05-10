#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <set>
#include <utility>

#include <algo/processes/requests.h>
#include <algo/processes/strategies.h>
#include <algo/processes/types.h>
#include <utils/tasks.h>

#include "../rand_utils.h"

#include "task_abstract_generator.h"

namespace Generators::ProcessesTask::TaskGenerators {
using namespace ProcessesManagement;
using std::nullopt;
using std::optional;
using std::set;
using std::vector;

class WinNtTaskGenerator : public AbstractTaskGenerator {
public:
  WinNtTaskGenerator() = default;

  bool preemptive() const override { return true; }

  ~WinNtTaskGenerator() override = default;

  optional<Request> CreateProcessReq(const ProcessesState &state,
                                     bool valid = true) const override {
    auto [processes, queues] = state;
    auto usedPids = getUsedPids(state);
    auto availablePids = getAvailablePids(state);
    auto priority = RandUtils::randRange(0, 11);

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
      }
      return ProcessesManagement::CreateProcessReq(pid, ppid, priority,
                                                   priority);
    } else if (!valid && !usedPids.empty()) {
      auto pid = RandUtils::randChoice(usedPids.begin(), usedPids.end());
      return ProcessesManagement::CreateProcessReq(pid, -1, priority, priority);
    } else {
      return nullopt;
    }
  }

  optional<Request> TerminateProcessReq(const ProcessesState &state,
                                        bool valid = true) const override {
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

  optional<Request> InitIO(const ProcessesState &state,
                           bool valid = true) const override {
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

  optional<Request> TerminateIO(const ProcessesState &state,
                                bool valid = true) const override {
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
      auto augment = RandUtils::randRange(1, 3);
      return ProcessesManagement::TerminateIO(pid, augment);
    } else if (!valid && !otherPids.empty()) {
      auto pid = RandUtils::randChoice(otherPids.begin(), otherPids.end());
      auto augment = RandUtils::randRange(1, 3);
      return ProcessesManagement::TerminateIO(pid, augment);
    } else {
      return nullopt;
    }
  }

  optional<Request> TransferControl(const ProcessesState &state,
                                    bool valid = true) const override {
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
};
} // namespace Generators::ProcessesTask::TaskGenerators
