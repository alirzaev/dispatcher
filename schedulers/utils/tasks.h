#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <variant>
#include <vector>

#include <nlohmann/json.hpp>

#include "../algo/memory/exceptions.h"
#include "../algo/memory/requests.h"
#include "../algo/memory/strategies.h"
#include "../algo/processes/exceptions.h"
#include "../algo/processes/requests.h"
#include "../algo/processes/strategies.h"
#include "exceptions.h"

namespace Utils {
namespace Memory = MemoryManagement;
namespace Processes = ProcessesManagement;

class MemoryTask {
private:
  Memory::StrategyPtr _strategy;

  uint32_t _completed;

  Memory::MemoryState _state;

  std::vector<Memory::Request> _requests;

  MemoryTask(Memory::StrategyPtr strategy, uint32_t completed,
             const Memory::MemoryState &state,
             const std::vector<Memory::Request> requests)
      : _strategy(strategy), _completed(completed), _state(state),
        _requests(requests) {}

public:
  static MemoryTask create(Memory::StrategyPtr strategy, uint32_t completed,
                           const Memory::MemoryState &state,
                           const std::vector<Memory::Request> requests) {
    return {strategy, completed, state, requests};
  }

  static void validate(Memory::StrategyPtr strategy, uint32_t completed,
                       const Memory::MemoryState &state,
                       const std::vector<Memory::Request> requests) {
    if (requests.size() < completed) {
      throw TaskException("COMPLETED_OOR");
    }
    auto currentState = Memory::MemoryState::initial();
    try {
      for (auto req = requests.begin(); req != requests.begin() + completed;
           ++req) {
        currentState = strategy->processRequest(*req, currentState);
      }
      if (currentState != state) {
        throw TaskException("STATE_MISMATCH");
      }
    } catch (Memory::BaseException &ex) {
      throw TaskException(ex.what());
    }
  }

  Memory::StrategyPtr strategy() const { return _strategy; }

  // кол-во выполненных заданий
  uint32_t completed() const { return _completed; }

  const Memory::MemoryState &state() const { return _state; }

  const std::vector<Memory::Request> &requests() const { return _requests; }

  nlohmann::json dump() const {
    nlohmann::json obj;

    obj["type"] = "MEMORY_TASK";

    obj["strategy"] = strategy()->toString();

    obj["completed"] = completed();

    obj["state"] = state().dump();

    obj["requests"] = nlohmann::json::array();

    for (auto request : requests()) {
      std::visit(
          [&obj](const auto &request) {
            obj["requests"].push_back(request.dump());
          },
          request);
    }

    return obj;
  }

  bool done() const { return _completed == _requests.size(); }

  std::optional<MemoryTask> next(const Memory::MemoryState &state) const {
    if (done()) {
      return *this;
    }
    try {
      auto request = _requests[_completed];
      auto expected = _strategy->processRequest(request, _state);
      if (expected == state) {
        return MemoryTask{_strategy, _completed + 1, expected, _requests};
      } else {
        return std::nullopt;
      }
    } catch (...) {
      return std::nullopt;
    }
  }
}; // namespace Process=ProceclassMemoryTask

class ProcessesTask {
private:
  Processes::StrategyPtr _strategy;

  uint32_t _completed;

  Processes::ProcessesState _state;

  std::vector<Processes::Request> _requests;

  ProcessesTask(Processes::StrategyPtr strategy, uint32_t completed,
                const Processes::ProcessesState &state,
                const std::vector<Processes::Request> requests)
      : _strategy(strategy), _completed(completed), _state(state),
        _requests(requests) {}

public:
  static ProcessesTask create(Processes::StrategyPtr strategy,
                              uint32_t completed,
                              const Processes::ProcessesState &state,
                              const std::vector<Processes::Request> requests) {
    return {strategy, completed, state, requests};
  }

  nlohmann::json dump() const {
    nlohmann::json obj;

    obj["type"] = "PROCESSES_TASK";

    obj["strategy"] = strategy()->toString();

    obj["completed"] = completed();

    obj["state"] = state().dump();

    obj["requests"] = nlohmann::json::array();

    for (auto request : requests()) {
      std::visit(
          [&obj](const auto &request) {
            obj["requests"].push_back(request.dump());
          },
          request);
    }

    return obj;
  }

  Processes::StrategyPtr strategy() const { return _strategy; }

  // кол-во выполненных заданий
  uint32_t completed() const { return _completed; }

  const Processes::ProcessesState &state() const { return _state; }

  const std::vector<Processes::Request> &requests() const { return _requests; }

  bool done() const { return _completed == _requests.size(); }

  std::optional<ProcessesTask>
  next(const Processes::ProcessesState &state) const {
    if (done()) {
      return *this;
    }
    try {
      auto request = _requests[_completed];
      auto expected = _strategy->processRequest(request, _state);
      if (expected == state) {
        return ProcessesTask{_strategy, _completed + 1, expected, _requests};
      } else {
        return std::nullopt;
      }
    } catch (...) {
      return std::nullopt;
    }
  }
};

using Task = std::variant<MemoryTask, ProcessesTask>;
} // namespace Utils
