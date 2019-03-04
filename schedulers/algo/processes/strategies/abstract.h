#pragma once

#include <algorithm>
#include <cstdint>
#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include "../operations.h"
#include "../requests.h"
#include "../types.h"

namespace ProcessesManagement {

enum class StrategyType { ROUNDROBIN };

class AbstractStrategy {
public:
  virtual ~AbstractStrategy() {}

  AbstractStrategy(StrategyType type) : type(type) {}

  const StrategyType type;

  virtual std::string toString() const = 0;

  ProcessesState processRequest(const Request &request,
                                const ProcessesState &state) const {
    return std::visit(
        [this, state](const auto &req) {
          return updateTimer(this->processRequest(req, state));
        },
        request);
  }

protected:
  virtual ProcessesState processRequest(const CreateProcessReq &request,
                                        const ProcessesState &state) const = 0;

  virtual ProcessesState processRequest(const TerminateProcessReq &request,
                                        const ProcessesState &state) const = 0;

  virtual ProcessesState processRequest(const InitIO &request,
                                        const ProcessesState &state) const = 0;

  virtual ProcessesState processRequest(const TerminateIO &request,
                                        const ProcessesState &state) const = 0;

  virtual ProcessesState processRequest(const TransferControl &request,
                                        const ProcessesState &state) const = 0;

  virtual ProcessesState processRequest(const TimeQuantumExpired &request,
                                        const ProcessesState &state) const = 0;

  virtual std::optional<std::pair<int32_t, int32_t>>
  schedule(const ProcessesState &state) const = 0;

  std::optional<Process> getCurrent(const ProcessesState &state) const {
    auto current =
        std::find_if(state.processes.begin(), state.processes.end(),
                     [](const auto &process) {
                       return process.state() == ProcState::EXECUTING;
                     });
    if (current == state.processes.end()) {
      return std::nullopt;
    } else {
      return {*current};
    };
  }

  std::optional<Process> getProcessByPid(const ProcessesState &state,
                                         int32_t pid) const {
    if (auto it = std::find_if(
            state.processes.begin(), state.processes.end(),
            [&pid](const auto &process) { return process.pid() == pid; });
        it != state.processes.end()) {
      return {*it};
    } else {
      return std::nullopt;
    }
  }
};

using StrategyPtr = std::shared_ptr<AbstractStrategy>;
} // namespace ProcessesManagement
