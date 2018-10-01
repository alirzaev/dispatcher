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
            } else if (request->type == Requests::RequestType::TERMINATE_PROCESS) {
                auto obj = *dynamic_cast<Requests::TerminateProcess*>(request.get());
                return terminateProcess(obj, state);
            } else if (request->type == Requests::RequestType::ALLOCATE_MEMORY) {
                auto obj = *dynamic_cast<Requests::AllocateMemory*>(request.get());
                return allocateMemory(obj, state);
            } else {
                throw std::exception();
            }
        }

        Types::MemoryState createProcess(
                const Requests::CreateProcess request,
                const Types::MemoryState& state
        ) const
        {
            std::vector<Types::MemoryBlock> blocks, freeBlocks;
            std::tie(blocks, freeBlocks) = state;

            // проверить, выделены ли процессу какие-либо блоки памяти
            auto processExists = std::find_if(
                        blocks.begin(),
                        blocks.end(),
                        [&request](const Types::MemoryBlock& block) {
                return request.pid == block.pid();
            }) != blocks.end();
            // если процессу уже выделены блоки памяти, значит он уже создан,
            // заявку проигнорировать
            if (processExists) {
                return state;
            }

            // найти первый подходящий блок памяти, т. е. тот, размер которого
            // больше запрашиваемой процессом памяти
            auto freePos = std::find_if(
                        freeBlocks.begin(),
                        freeBlocks.end(),
                        [&request](const Types::MemoryBlock& block) {
                return block.size() > request.pages;
            });

            // если нет подходящего блока - игнорируем заявку
            if (freePos == freeBlocks.end()) {
                return state;
            }

            // выделить процессу памяти
            auto pos = std::find(blocks.begin(), blocks.end(), *freePos);
            uint32_t index = pos - blocks.begin();

            return Operations::allocateMemory(state, index, request.pid, request.pages);
        }

        Types::MemoryState terminateProcess(
                const Requests::TerminateProcess request,
                const Types::MemoryState& state
        ) const
        {
            std::vector<Types::MemoryBlock> blocks, freeBlocks;
            MemoryState currentState = state;
            std::tie(blocks, freeBlocks) = currentState;

            while (true) {
                // ищем очередной блок памяти, выделенный процессу
                auto pos = std::find_if(
                            blocks.begin(),
                            blocks.end(),
                            [&request](const Types::MemoryBlock& block) {
                    return request.pid == block.pid();
                });
                if (pos == blocks.end()) {
                    break;
                }

                // освобождаем блок
                uint32_t index = pos - blocks.begin();
                currentState = Operations::freeMemory(currentState, request.pid, index);
                std::tie(blocks, freeBlocks) = currentState;
            }

            // сжатие памяти
            while (true) {
                // ищем первый свободный блок памяти
                // проверяем, есть ли за ним хотя бы один свободный блок
                uint32_t index = 0;
                for (; index < blocks.size() - 1 &&
                     !(blocks[index].pid() == -1 &&
                      blocks[index + 1].pid() == -1); ++index) {}

                // если есть, то выполняем сжатие
                if (index < blocks.size() - 1) {
                    currentState = Operations::compressMemory(currentState, index);
                    std::tie(blocks, freeBlocks) = currentState;
                } else {
                    break;
                }
            }

            return currentState;
        }

        Types::MemoryState allocateMemory(
                const Requests::AllocateMemory request,
                const Types::MemoryState& state
        ) const
        {
            std::vector<Types::MemoryBlock> blocks, freeBlocks;
            std::tie(blocks, freeBlocks) = state;

            // проверить, выделены ли процессу какие-либо блоки памяти
            auto processPos = std::find_if(
                        blocks.begin(),
                        blocks.end(),
                        [&request](const Types::MemoryBlock& block) {
                return block.pid() == request.pid;
            });
            // если не выделены, то заявку проигнорировать
            if (processPos == blocks.end()) {
                return state;
            }

            int32_t totalFree = 0;
            for (const auto& block : freeBlocks) {
                totalFree += block.size();
            }
            std::vector<Types::MemoryBlock>::const_iterator pos;
            // проверить, если свободный блок подходящего размера
            auto freeBlockPos = std::find_if(
                        freeBlocks.begin(),
                        freeBlocks.end(),
                        [&request](const Types::MemoryBlock& block) {
                return request.pages < block.size();
            });
            // если есть, то выделить процессу память в этом блоке
            if ((pos = findFreeBlock(blocks, freeBlocks, request.pages)) != blocks.end()) {
                uint32_t index = pos - blocks.cbegin();
                return Operations::allocateMemory(state, index, request.pid, request.pages);
            } else if (totalFree > request.pages) { // если суммарно свободной памяти достаточно, то выполнить дефрагментацию
                auto newState = Operations::defragmentMemory(state);
                std::tie(blocks, freeBlocks) = newState;
                pos = findFreeBlock(blocks, freeBlocks, request.pages);
                uint32_t index = pos - blocks.cbegin();
                return Operations::allocateMemory(newState, index, request.pid, request.pages);
            } else { // недостаточно свободной памяти, проигнорировать заявку
                return state;
            }
        }
    private:
        FirstAppropriateStrategy() :
            AbstractStrategy(StrategyType::FIRST_APPROPRIATE)
        {}
        std::vector<Types::MemoryBlock>::const_iterator findFreeBlock(
                const std::vector<Types::MemoryBlock>& blocks,
                const std::vector<Types::MemoryBlock>& freeBlocks,
                int32_t size
        ) const
        {
            auto freeBlockPos = std::find_if(
                        freeBlocks.cbegin(),
                        freeBlocks.cend(),
                        [&size](const Types::MemoryBlock& block) {
                return size < block.size();
            });
            if (freeBlockPos != freeBlocks.cend()) {
                return std::find(blocks.cbegin(), blocks.cend(), *freeBlockPos);
            } else {
                return blocks.cend();
            }
        }
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
