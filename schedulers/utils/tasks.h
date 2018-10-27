#ifndef TASKS_H
#define TASKS_H

#include <memory>
#include <cstdint>
#include <vector>
#include <variant>
#include <stdexcept>
#include <optional>

#include <nlohmann/json.hpp>

#include "../algo/memory/requests.h"
#include "../algo/memory/strategies.h"
#include "../algo/memory/exceptions.h"
#include "exceptions.h"

namespace Utils::Tasks
{
using namespace MemoryManagement::Strategies;
using namespace MemoryManagement::Types;
using namespace MemoryManagement::Requests;
using namespace Exceptions;

class MemoryTask
{
private:
    StrategyPtr _strategy;

    uint32_t _completed;

    MemoryState _state;

    std::vector<RequestPtr> _requests;

    MemoryTask(
            StrategyPtr strategy,
            uint32_t completed,
            const MemoryState& state,
            const std::vector<RequestPtr> requests
    ) :
        _strategy(strategy),
        _completed(completed),
        _state(state),
        _requests(requests)
    { }

public:
    static MemoryTask create(
            StrategyPtr strategy,
            uint32_t completed,
            const MemoryState& state,
            const std::vector<RequestPtr> requests
    )
    {
        return { strategy, completed, state, requests };
    }

    static void validate(
            StrategyPtr strategy,
            uint32_t completed,
            const MemoryState& state,
            const std::vector<RequestPtr> requests
    )
    {
        if (requests.size() < completed) {
            throw TaskException("COMPLETED_OOR");
        }
        auto currentState = MemoryState::initial();
        try {
            for (
                 auto req = requests.begin();
                 req != requests.begin() + completed;
                 ++req
            ) {
                currentState = strategy->processRequest(*req, currentState);
            }
            if (currentState != state) {
                throw TaskException("STATE_MISMATCH");
            }
        } catch (MemoryManagement::Exceptions::BaseException& ex) {
            throw TaskException(ex.what());
        }
    }

    StrategyPtr strategy() const
    {
        return _strategy;
    }

    // кол-во выполненных заданий
    uint32_t completed() const
    {
        return _completed;
    }

    const MemoryState& state() const
    {
        return _state;
    }

    const std::vector<RequestPtr>& requests() const
    {
        return _requests;
    }

    nlohmann::json dump() const
    {
        nlohmann::json obj;

        obj["type"] = "MEMORY_TASK";

        obj["strategy"] = strategy()->toString();

        obj["completed"] = completed();

        obj["state"] = state().dump();

        auto jsonRequests = nlohmann::json::array();

        for (auto request : requests())
        {
            jsonRequests.push_back(request->dump());
        }

        obj["requests"] = jsonRequests;

        return obj;
    }

    bool done() const
    {
        return _completed == _requests.size();
    }

    std::optional<MemoryTask> next(const MemoryState& state) const
    {
        if (done())
        {
            return *this;
        }
        try
        {
            auto request = _requests[_completed];
            auto expected = _strategy->processRequest(request, _state);
            if (expected == state)
            {
                return MemoryTask{_strategy, _completed + 1, expected, _requests};
            }
            else
            {
                return std::nullopt;
            }
        }
        catch (const std::exception& ex)
        {
            return std::nullopt;
        }
    }
};

class ProcessesTask
{ };

using Task = std::variant<MemoryTask, ProcessesTask>;
}

#endif // TASKS_H
