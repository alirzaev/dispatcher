#pragma once

#include <algorithm>

#include "operations.h"
#include "strategies.h"

using std::nullopt;
using std::optional;
using std::pair;
using std::shared_ptr;

namespace ProcessesManagement {
std::string RoundRobinStrategy::toString() const { return "ROUNDROBIN"; }

shared_ptr<RoundRobinStrategy> RoundRobinStrategy::create() {
  return shared_ptr<RoundRobinStrategy>(new RoundRobinStrategy());
}

optional<pair<int32_t, int32_t>>
RoundRobinStrategy::schedule(const ProcessesState &state) const {
  auto [processes, queues] = state;

  if (queues[0].empty()) {
    return nullopt;
  } else {
    auto pid = queues[0].front();
    return {{pid, 0}};
  }
}

RoundRobinStrategy::RoundRobinStrategy()
    : AbstractStrategy(StrategyType::ROUNDROBIN) {}

ProcessesState
RoundRobinStrategy::processRequest(const CreateProcessReq &request,
                                   const ProcessesState &state) const {
  auto newState = state;
  auto process = Process{}
                     .pid(request.pid())
                     .ppid(request.ppid())
                     .priority(request.priority())
                     .basePriority(request.basePriority())
                     .timer(request.timer())
                     .workTime(request.workTime());
  newState.processes.push_back(process);
  std::sort(newState.processes.begin(), newState.processes.end());
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

ProcessesState
RoundRobinStrategy::processRequest(const TerminateProcessReq &request,
                                   const ProcessesState &state) const {
  auto newState = state;
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

ProcessesState
RoundRobinStrategy::processRequest(const InitIO &request,
                                   const ProcessesState &state) const {
  auto newState = state;
  newState = changeProcessState(newState, request.pid(), ProcState::WAITING);
  auto next = schedule(newState);
  if (next.has_value()) {
    auto [pid, queue] = next.value();
    newState = popFromQueue(newState, queue);
    newState = switchTo(newState, pid);
  }
  return newState;
}

ProcessesState
RoundRobinStrategy::processRequest(const TerminateIO &request,
                                   const ProcessesState &state) const {
  auto newState = state;
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

ProcessesState
RoundRobinStrategy::processRequest(const TransferControl &request,
                                   const ProcessesState &state) const {
  auto newState = state;
  newState = pushToQueue(newState, 0, request.pid());
  auto next = schedule(newState);
  if (next.has_value()) {
    auto [pid, queue] = next.value();
    newState = popFromQueue(newState, queue);
    newState = switchTo(newState, pid);
  }
  return newState;
}

ProcessesState
RoundRobinStrategy::processRequest(const TimeQuantumExpired &request,
                                   const ProcessesState &state) const {
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
} // namespace ProcessesManagement
