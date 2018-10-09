#ifndef TASKS_H
#define TASKS_H

#include <memory>
#include <cstdint>
#include <vector>
#include <stdexcept>

#include "../libs/nlohmann/json.hpp"

#include "../algo/memory/requests.h"
#include "../algo/memory/strategies.h"
#include "../algo/memory/exceptions.h"

namespace Utils {
namespace Tasks {
    using namespace MemoryManagement::Strategies;
    using namespace MemoryManagement::Types;
    using namespace MemoryManagement::Requests;

    enum class TaskType {
        MEMORY_TASK,
        PROCESS_TASK
    };

    class AbstractTask {
    public:
        AbstractTask(TaskType type) :
            type(type)
        {}

        const TaskType type;

        virtual nlohmann::json dump() const = 0;

        virtual ~AbstractTask()
        {}
    };

    using TaskPtr = std::shared_ptr<AbstractTask>;

    class MemoryTask : public AbstractTask {
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
            AbstractTask(TaskType::MEMORY_TASK),
            _strategy(strategy),
            _completed(completed),
            _state(state),
            _requests(requests)
        {}

        nlohmann::json dump(const MemoryBlock& block) const
        {
            return {
                {"pid", block.pid()},
                {"address", block.address()},
                {"size", block.size()}
            };
        }
    public:
        static std::shared_ptr<MemoryTask> create(
                StrategyPtr strategy,
                uint32_t completed,
                const MemoryState& state,
                const std::vector<RequestPtr> requests
        )
        {
            return std::shared_ptr<MemoryTask>(
                new MemoryTask(strategy, completed, state, requests)
            );
        }

        static void validate(
                StrategyPtr strategy,
                uint32_t completed,
                const MemoryState& state,
                const std::vector<RequestPtr> requests
        )
        {
            if (requests.size() < completed) {
                throw std::logic_error("COMPLETED_OOR");
            }
            auto currentState = MemoryState{
                {MemoryBlock{-1, 0, 256}},
                {MemoryBlock{-1, 0, 256}}
            };
            try {
                for (
                     auto req = requests.begin();
                     req != requests.begin() + completed;
                     ++req
                ) {
                    currentState = strategy->processRequest(*req, currentState);
                }
                if (currentState != state) {
                    throw std::logic_error("STATE_MISMATCH");
                }
            } catch (MemoryManagement::Exceptions::BaseException& ex) {
                throw std::logic_error(ex.what());
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

        nlohmann::json dump() const override
        {
            nlohmann::json obj;

            obj["strategy"] = strategy()->toString();

            obj["completed"] = completed();

            auto [blocks, freeBlocks] = state();
            auto jsonBlocks = nlohmann::json::array();
            auto jsonFreeBlocks = nlohmann::json::array();

            for (const auto& block : blocks) {
                jsonBlocks.push_back(dump(block));
            }
            for (const auto& block : freeBlocks) {
                jsonFreeBlocks.push_back(dump(block));
            }
            obj["state"] = {
                {"blocks", jsonBlocks},
                {"free_blocks", jsonFreeBlocks}
            };

            auto jsonRequests = nlohmann::json::array();

            for (auto request : requests()) {
                jsonRequests.push_back(request->dump());
            }

            obj["requests"] = jsonRequests;

            return obj;
        }
    };
}
}

#endif // TASKS_H
