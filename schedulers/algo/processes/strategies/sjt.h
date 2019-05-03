#pragma once

#include <algorithm>
#include <cstdint>
#include <exception>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <variant>

#include "../../../utils/overload.h"
#include "../exceptions.h"
#include "../operations.h"
#include "abstract.h"

namespace ProcessesManagement {
/**
 *  @brief Стратегия "SJT".
 */
class SjtStrategy final : public AbstractStrategy {
public:
  std::string toString() const override { return "SJT"; }

  static std::shared_ptr<SjtStrategy> create() {
    return std::shared_ptr<SjtStrategy>(new SjtStrategy());
  }

  std::string getRequestDescription(const Request &request) const override {
    using namespace std::string_literals;
    using ss = std::stringstream;

    auto base = AbstractStrategy::getRequestDescription(request);

    if (std::holds_alternative<CreateProcessReq>(request)) {
      auto req = std::get<CreateProcessReq>(request);
      return static_cast<const ss &>(ss() << base << ". "
                                          << "Предполагаемое время выполнения: "
                                          << req.workTime())
          .str();
    } else {
      return base;
    }
  }

protected:
  std::optional<std::pair<int32_t, int32_t>>
  schedule(const ProcessesState &state) const override {
    auto [processes, queues] = state;

    if (!queues[0].empty()) {
      auto pid = queues[0].front();
      return {{pid, 0}};
    } else {
      return std::nullopt;
    }
  }

private:
  SjtStrategy() : AbstractStrategy(StrategyType::SJT) {}

  ProcessesState sortQueues(const ProcessesState &state) const {
    auto [processes, queues] = state;

    std::deque<int32_t> queue;
    std::map<int32_t, Process> processesMap;

    for (const auto &process : processes) {
      processesMap[process.pid()] = process;
    }

    for (int32_t pid : queues[0]) {
      const auto &process = processesMap.at(pid);
      if (process.workTime() >= process.timer()) {
        queue.push_back(process.pid());
      }
    }

    std::stable_sort(queue.begin(), queue.end(),
                     [&processesMap](int32_t left, int32_t right) {
                       auto leftTime = processesMap[left].workTime();
                       auto rightTime = processesMap[right].workTime();

                       return leftTime < rightTime;
                     });

    for (int32_t pid : queues[0]) {
      const auto &process = processesMap.at(pid);
      if (process.workTime() < process.timer()) {
        queue.push_back(process.pid());
      }
    }

    queues[0] = queue;

    return {processes, queues};
  }

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
    newState = sortQueues(newState);

    if (newState.processes.size() == 1) {
      auto next = schedule(newState);
      if (next) {
        auto [pid, queue] = next.value();
        newState = popFromQueue(newState, queue);
        newState = switchTo(newState, pid);
      }
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
    auto process = getProcessByPid(newState, request.pid());

    if (!process) {
      return newState;
    }
    if (process->state() != ProcState::EXECUTING) {
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
    auto process = getProcessByPid(newState, request.pid());

    if (!process) {
      return newState;
    }
    if (process->state() != ProcState::WAITING) {
      return newState;
    }

    newState = pushToQueue(newState, 0, request.pid());
    newState = sortQueues(newState);
    newState = changeProcessState(newState, request.pid(), ProcState::ACTIVE);

    auto current = getCurrent(newState);
    auto next = schedule(newState);
    if (!current && next) {
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
    newState = sortQueues(newState);

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
    return state;
  }
};
} // namespace ProcessesManagement
