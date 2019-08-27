#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <string>
#include <utility>

#include <mapbox/variant.hpp>
#include <tl/optional.hpp>

#include "../exceptions.h"
#include "../operations.h"
#include "abstract.h"

namespace ProcessesManagement {
/**
 *  @brief Стратегия "WinNT".
 */
class UnixStrategy final : public AbstractStrategy {
public:
  StrategyType type() const override { return StrategyType::UNIX; }

  std::string toString() const override { return "UNIX"; }

  static std::shared_ptr<UnixStrategy> create() {
    return std::shared_ptr<UnixStrategy>(new UnixStrategy());
  }

  std::string getRequestDescription(const Request &request) const override {
    using namespace std::string_literals;
    using ss = std::stringstream;

    auto base = AbstractStrategy::getRequestDescription(request);

    if (request.is<CreateProcessReq>()) {
      auto req = request.get<CreateProcessReq>();
      return static_cast<const ss &>(ss() << base << ". "
                                          << "Приоритет: " << req.priority())
          .str();
    } else {
      return base;
    }
  }

protected:
  tl::optional<std::pair<int32_t, size_t>>
  schedule(const ProcessesState &state) const override {
    auto [processes, queues] = state;

    for (size_t j = 0, i = 15; j < queues.size(); ++j, --i) {
      if (!queues[i].empty()) {
        auto pid = queues[i].front();
        return {{pid, i}};
      }
    }
    return tl::nullopt;
  }

private:
  UnixStrategy() : AbstractStrategy() {}

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
    newState = pushToQueue(newState, process.priority(), process.pid());

    auto current = getCurrent(newState);
    auto next = schedule(newState);

    if (next) {
      auto [pid, queue] = next.value();
      auto processIndex = getIndexByPid(newState, pid);
      auto process = newState.processes.at(*processIndex);

      if (!current) {
        newState = popFromQueue(newState, queue);
        newState = switchTo(newState, pid);
      } else if (current && process.priority() > current->priority()) {
        newState = pushToQueue(newState, current->priority(), current->pid());
        newState = popFromQueue(newState, queue);
        newState = switchTo(newState, pid);
      }
    } else {
      return newState;
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
    if (!current && next) {
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
    if (next) {
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
    auto process = newState.processes.at(*processIndex);
    if (process.state() != ProcState::WAITING) {
      return newState;
    }

    newState = pushToQueue(newState, process.priority(), request.pid());
    newState = changeProcessState(newState, request.pid(), ProcState::ACTIVE);

    auto current = getCurrent(newState);
    auto next = schedule(newState);

    if (next) {
      auto [pid, queue] = next.value();
      auto processIndex = getIndexByPid(newState, pid);
      auto process = newState.processes.at(*processIndex);

      if (!current) {
        newState = popFromQueue(newState, queue);
        newState = switchTo(newState, pid);
      } else if (current && process.priority() > current->priority()) {
        newState = pushToQueue(newState, current->priority(), current->pid());
        newState = popFromQueue(newState, queue);
        newState = switchTo(newState, pid);
      }
    } else {
      return newState;
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
    auto process = newState.processes.at(*processIndex);
    if (process.state() != ProcState::EXECUTING) {
      return newState;
    }

    newState = pushToQueue(newState, process.priority(), request.pid());

    auto next = schedule(newState);
    if (next) {
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
    if (current) {
      newState = pushToQueue(newState, current->priority(), current->pid());
    }
    auto next = schedule(newState);
    if (next) {
      auto [pid, queue] = next.value();
      newState = popFromQueue(newState, queue);
      newState = switchTo(newState, pid);
    }
    return newState;
  }
}; // namespace ProcessesManagement
} // namespace ProcessesManagement
