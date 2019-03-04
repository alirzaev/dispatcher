#pragma once

#include <algorithm>
#include <cstdint>
#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include "../exceptions.h"
#include "../operations.h"
#include "abstract.h"

namespace ProcessesManagement {
class RoundRobinStrategy final : public AbstractStrategy {
public:
  std::string toString() const override { return "ROUNDROBIN"; }

  static std::shared_ptr<RoundRobinStrategy> create() {
    return std::shared_ptr<RoundRobinStrategy>(new RoundRobinStrategy());
  }

protected:
  // <pid, queue>
  std::optional<std::pair<int32_t, int32_t>>
  schedule(const ProcessesState &state) const override {
    auto [processes, queues] = state;

    if (queues[0].empty()) {
      return std::nullopt;
    } else {
      auto pid = queues[0].front();
      return {{pid, 0}};
    }
  }

private:
  RoundRobinStrategy() : AbstractStrategy(StrategyType::ROUNDROBIN) {}

protected:
  ProcessesState processRequest(const CreateProcessReq &request,
                                const ProcessesState &state) const override {
    auto newState = state;
    auto process = request.toProcess();

    if (getProcessByPid(newState, process.pid())) {
      return newState;
    }
    auto parent = getProcessByPid(newState, process.ppid());
    if (process.ppid() != -1) {
      if (!parent.has_value()) {
        return newState;
      }
      if (parent->state() != ProcState::EXECUTING) {
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

    if (!getProcessByPid(newState, request.pid())) {
      return newState;
    }

    newState = terminateProcess(newState, request.pid());

    auto current = getCurrent(newState);
    auto next = schedule(newState);
    if (!current.has_value() && next.has_value()) {
      auto [pid, queue] = next.value();
      newState = popFromQueue(newState, queue);
      newState = switchTo(newState, pid);
    }
    return newState;
  }
  ProcessesState processRequest(const InitIO &request,
                                const ProcessesState &state) const override {
    auto newState = state;
    auto process = getProcessByPid(newState, request.pid());

    if (!process) {
      return newState;
    }
    if (process->state() != ProcState::EXECUTING) {
      return newState;
    }

    newState = changeProcessState(newState, request.pid(), ProcState::WAITING);

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
    auto process = getProcessByPid(newState, request.pid());

    if (!process) {
      return newState;
    }
    if (process->state() != ProcState::WAITING) {
      return newState;
    }

    newState = pushToQueue(newState, 0, request.pid());
    newState = changeProcessState(newState, request.pid(), ProcState::ACTIVE);

    auto current = getCurrent(newState);
    auto next = schedule(newState);
    if (!current.has_value() && next.has_value()) {
      auto [pid, queue] = next.value();
      newState = popFromQueue(newState, queue);
      newState = switchTo(newState, pid);
    }
    return newState;
  }
  ProcessesState processRequest(const TransferControl &request,
                                const ProcessesState &state) const override {
    auto newState = state;
    auto process = getProcessByPid(newState, request.pid());

    if (!process) {
      return newState;
    }
    if (process->state() != ProcState::EXECUTING) {
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
      newState = pushToQueue(newState, 0, current->pid());
    }
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
