#pragma once

#include <algorithm>
#include <cstddef>
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
/**
 *  @brief Стратегия "RoundRobin".
 */
class RoundRobinStrategy final : public AbstractStrategy {
public:
  StrategyType type() const override { return StrategyType::ROUNDROBIN; }

  std::string toString() const override { return "ROUNDROBIN"; }

  static std::shared_ptr<RoundRobinStrategy> create() {
    return std::shared_ptr<RoundRobinStrategy>(new RoundRobinStrategy());
  }

protected:
  std::optional<std::pair<int32_t, size_t>>
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
  RoundRobinStrategy() : AbstractStrategy() {}

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
    auto processIndex = getIndexByPid(newState, request.pid());

    if (!processIndex) {
      return newState;
    }
    if (auto process = newState.processes.at(*processIndex);
        process.state() != ProcState::EXECUTING) {
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
