#ifndef TASKS_H
#define TASKS_H

#include <memory>
#include <cstdint>
#include <vector>

#include "../algo/memory/requests.h"
#include "../algo/memory/strategies.h"

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

        StrategyPtr strategy() const
        {
            return _strategy;
        }

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
    };
}
}

#endif // TASKS_H
