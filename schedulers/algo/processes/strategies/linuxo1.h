#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

#include <tl/optional.hpp>

#include "../exceptions.h"
#include "../operations.h"
#include "abstract.h"

namespace ProcessesManagement {
/**
 *  @brief Стратегия "Linux O(1)".
 */
class LinuxO1Strategy final : public AbstractStrategy {
public:
  StrategyType type() const override { return StrategyType::LINUXO1; }

  std::string toString() const override { return "LINUXO1"; }

  static std::shared_ptr<LinuxO1Strategy> create() {
    return std::shared_ptr<LinuxO1Strategy>(new LinuxO1Strategy());
  }

protected:
  tl::optional<std::pair<int32_t, size_t>>
  schedule(const ProcessesState &state) const override {
    auto [processes, queues] = state;

    if (queues[0].empty()) {
      return tl::nullopt;
    } else {
      auto pid = queues[0].front();
      return {{pid, 0}};
    }
  }

private:
  LinuxO1Strategy() : AbstractStrategy() {}

  ProcessesState exhangeQueues(const ProcessesState &state) const {
    if (state.queues[1].empty() || !state.queues[0].empty()) {
      return state;
    }

    auto newState = state;
    auto pids = newState.queues[1];

    for (auto pid : pids) {
      newState = popFromQueue(newState, 1);
      newState = pushToQueue(newState, 0, pid);
    }

    return newState;
  }

protected:
  ProcessesState processRequest(const CreateProcessReq &request,
                                const ProcessesState &state) const override {
    auto newState = state;
    auto process = request.toProcess();

    if (getIndexByPid(newState, process.pid())) {
      return newState;
    }
    auto parentIndex = getIndexByPid(newState, process.ppid());
    if (process.ppid() != -1) {
      if (!parentIndex.has_value()) {
        return newState;
      }
      if (auto parent = newState.processes.at(*parentIndex);
          parent.state() != ProcState::EXECUTING) {
        return newState;
      }
    }

    newState = addProcess(newState, process);
    newState = pushToQueue(newState, 0, process.pid());

    auto current = getCurrent(newState);
    auto next = schedule(newState);
    if (!current.has_value() && next.has_value()) {
      auto [pid, queue] = next.value();
      newState = popFromQueue(newState, queue);
      newState = switchTo(newState, pid);
    }
    return newState;
  }

  ProcessesState processRequest(const TerminateProcessReq &request,
                                const ProcessesState &state) const override {
    auto newState = state;

    if (!getIndexByPid(newState, request.pid())) {
      return newState;
    }

    newState = terminateProcess(newState, request.pid());

    auto current = getCurrent(newState);
    if (!current.has_value()) {
      newState = exhangeQueues(newState);
      auto next = schedule(newState);
      if (next.has_value()) {
        auto [pid, queue] = next.value();
        newState = popFromQueue(newState, queue);
        newState = switchTo(newState, pid);
      }
    }
    return newState;
  }

  ProcessesState processRequest(const InitIO &request,
                                const ProcessesState &state) const override {
    auto newState = state;
    auto processIndex = getIndexByPid(newState, request.pid());

    if (!processIndex) {
      return newState;
    }
    if (auto process = newState.processes.at(*processIndex);
        process.state() != ProcState::EXECUTING) {
      return newState;
    }

    newState = changeProcessState(newState, request.pid(), ProcState::WAITING);

    newState = exhangeQueues(newState);
    auto next = schedule(newState);
    if (next.has_value()) {
      auto [pid, queue] = next.value();
      newState = popFromQueue(newState, queue);
      newState = switchTo(newState, pid);
    }
    return newState;
  }

  ProcessesState processRequest(const TerminateIO &request,
                                const ProcessesState &state) const override {
    auto newState = state;
    auto processIndex = getIndexByPid(newState, request.pid());

    if (!processIndex) {
      return newState;
    }
    if (auto process = newState.processes.at(*processIndex);
        process.state() != ProcState::WAITING) {
      return newState;
    }

    newState = pushToQueue(newState, 0, request.pid());
    newState = changeProcessState(newState, request.pid(), ProcState::ACTIVE);

    auto current = getCurrent(newState);
    if (!current.has_value()) {
      newState = exhangeQueues(newState);
      auto next = schedule(newState);
      if (next.has_value()) {
        auto [pid, queue] = next.value();
        newState = popFromQueue(newState, queue);
        newState = switchTo(newState, pid);
      }
    }
    return newState;
  }

  ProcessesState processRequest(const TransferControl &request,
                                const ProcessesState &state) const override {
    auto newState = state;
    auto processIndex = getIndexByPid(newState, request.pid());

    if (!processIndex) {
      return newState;
    }
    if (auto process = newState.processes.at(*processIndex);
        process.state() != ProcState::EXECUTING) {
      return newState;
    }

    newState = pushToQueue(newState, 0, request.pid());

    auto next = schedule(newState);
    if (next.has_value()) {
      auto [pid, queue] = next.value();
      newState = popFromQueue(newState, queue);
      newState = switchTo(newState, pid);
    }
    return newState;
  }

  ProcessesState processRequest(const TimeQuantumExpired &,
                                const ProcessesState &state) const override {
    auto newState = state;
    auto current = getCurrent(newState);
    if (current.has_value()) {
      newState = pushToQueue(newState, 1, current->pid());
    }

    newState = exhangeQueues(newState);
    auto next = schedule(newState);
    if (next.has_value()) {
      auto [pid, queue] = next.value();
      newState = popFromQueue(newState, queue);
      newState = switchTo(newState, pid);
    }
    return newState;
  }
};
} // namespace ProcessesManagement
