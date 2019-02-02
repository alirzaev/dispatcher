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
#include "exceptions.h"

namespace Utils {
using namespace MemoryManagement;

class MemoryTask {
private:
  StrategyPtr _strategy;

  uint32_t _completed;

  MemoryState _state;

  std::vector<Request> _requests;

  MemoryTask(StrategyPtr strategy, uint32_t completed, const MemoryState &state,
             const std::vector<Request> requests)
      : _strategy(strategy), _completed(completed), _state(state),
        _requests(requests) {}

public:
  static MemoryTask create(StrategyPtr strategy, uint32_t completed,
                           const MemoryState &state,
                           const std::vector<Request> requests) {
    return {strategy, completed, state, requests};
  }

  static void validate(StrategyPtr strategy, uint32_t completed,
                       const MemoryState &state,
                       const std::vector<Request> requests) {
    if (requests.size() < completed) {
      throw TaskException("COMPLETED_OOR");
    }
    auto currentState = MemoryState::initial();
    try {
      for (auto req = requests.begin(); req != requests.begin() + completed;
           ++req) {
        currentState = strategy->processRequest(*req, currentState);
      }
      if (currentState != state) {
        throw TaskException("STATE_MISMATCH");
      }
    } catch (MemoryManagement::BaseException &ex) {
      throw TaskException(ex.what());
    }
  }

  StrategyPtr strategy() const { return _strategy; }

  // кол-во выполненных заданий
  uint32_t completed() const { return _completed; }

  const MemoryState &state() const { return _state; }

  const std::vector<Request> &requests() const { return _requests; }

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

  std::optional<MemoryTask> next(const MemoryState &state) const {
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
};

class ProcessesTask {
public:
  nlohmann::json dump() const {
    nlohmann::json obj;
    obj["type"] = "PROCESS_TASK";
    return obj;
  }
};

using Task = std::variant<MemoryTask, ProcessesTask>;
} // namespace Utils
