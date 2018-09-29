#ifndef STRATEGIES_H
#define STRATEGIES_H

#include "operations.h"
#include "requests.h"
#include "types.h"

#include <memory>
#include <algorithm>
#include <exception>
#include <cstdint>

namespace MemoryManagement {
namespace Strategies {
    enum class StrategyType {
        FIRST_APPROPRIATE,
        MOST_APPROPRIATE,
        LEAST_APPROPRIATE
    };

    class AbstractStrategy {
    public:
        AbstractStrategy(StrategyType type) :
            type(type)
        {}

        const StrategyType type;

        virtual Types::MemoryState processRequest(
                Requests::RequestPtr request,
                const Types::MemoryState state
        ) const = 0;

        virtual ~AbstractStrategy()
        {}
    };

    using StrategyPtr = std::shared_ptr<AbstractStrategy>;

    class FirstAppropriateStrategy final : public AbstractStrategy {
    public:
        static std::shared_ptr<FirstAppropriateStrategy> create()
        {
            return std::shared_ptr<FirstAppropriateStrategy>(new FirstAppropriateStrategy());
        }

        Types::MemoryState processRequest(
                Requests::RequestPtr request,
                const Types::MemoryState state
        ) const
        {
            if (request->type == Requests::RequestType::CREATE_PROCESS) {
                auto obj = *dynamic_cast<Requests::CreateProcess*>(request.get());
                return createProcess(obj, state);
            } else {
                throw std::exception();
            }
        }

        Types::MemoryState createProcess(
                const Requests::CreateProcess request,
                const Types::MemoryState state
        ) const
        {
            std::vector<Types::MemoryBlock> blocks, freeBlocks;
            std::tie(blocks, freeBlocks) = state;

            auto processExists = std::find_if(
                        blocks.begin(),
                        blocks.end(),
                        [&request](const Types::MemoryBlock& block) {
                return request.pid == block.pid();
            }) != blocks.end();
            if (processExists) {
                return state;
            }

            auto freePos = std::find_if(
                        freeBlocks.begin(),
                        freeBlocks.end(),
                        [&request](const Types::MemoryBlock& block) {
                return block.size() > request.pages;
            });
            if (freePos == freeBlocks.end()) {
                return state;
            }
            auto pos = std::find(blocks.begin(), blocks.end(), *freePos);
            uint32_t index = pos - blocks.begin();

            return Operations::allocateMemory(state, index, request.pid, request.pages);
        }
    private:
        FirstAppropriateStrategy() :
            AbstractStrategy(StrategyType::FIRST_APPROPRIATE)
        {}
    };

    class MostAppropriateStrategy final : public AbstractStrategy {
    public:
        static std::shared_ptr<MostAppropriateStrategy> create()
        {
            return std::shared_ptr<MostAppropriateStrategy>(new MostAppropriateStrategy());
        }

        Types::MemoryState processRequest(
                Requests::RequestPtr request,
                const Types::MemoryState state
        ) const
        {
            return state;
        }
    private:
        MostAppropriateStrategy() :
            AbstractStrategy(StrategyType::MOST_APPROPRIATE)
        {}
    };

    class LeastAppropriateStrategy final : public AbstractStrategy {
    public:
        static std::shared_ptr<LeastAppropriateStrategy> create()
        {
            return std::shared_ptr<LeastAppropriateStrategy>(new LeastAppropriateStrategy());
        }

        Types::MemoryState processRequest(
                Requests::RequestPtr request,
                const Types::MemoryState state
        ) const
        {
            return state;
        }
    private:
        LeastAppropriateStrategy() :
            AbstractStrategy(StrategyType::LEAST_APPROPRIATE)
        {}
    };
}
}

#endif // STRATEGIES_H
