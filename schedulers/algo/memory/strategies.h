#ifndef STRATEGIES_H
#define STRATEGIES_H

#include "operations.h"
#include "requests.h"
#include "types.h"

#include <memory>
#include <algorithm>
#include <exception>
#include <cstdint>
#include <string>

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

        virtual std::string toString() const = 0;

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
            } else if (request->type == Requests::RequestType::FREE_MEMORY) {
                auto obj = *dynamic_cast<Requests::FreeMemory*>(request.get());
                return freeMemory(obj, state);
            } else {
                throw std::exception();
            }
        }

        Types::MemoryState createProcess(
                const Requests::CreateProcess request,
                const Types::MemoryState& state
        ) const
        {
            return sortFreeBlocks(allocateMemoryGeneral(
                *Requests::AllocateMemory::create(request.pid, request.bytes, request.pages),
                state,
                true
            ));
        }

        Types::MemoryState terminateProcess(
                const Requests::TerminateProcess request,
                const Types::MemoryState& state
        ) const
        {
            auto currentState = state;

            while (true) {
                auto [blocks, freeBlocks] = currentState;
                // ищем очередной блок памяти, выделенный процессу
                auto pos = std::find_if(
                            blocks.begin(),
                            blocks.end(),
                            [&request](const auto& block) {
                    return request.pid == block.pid();
                });
                if (pos == blocks.end()) {
                    break;
                }

                // освобождаем блок
                uint32_t index = static_cast<uint32_t>(pos - blocks.begin());
                currentState = Operations::freeMemory(currentState, request.pid, index);
            }

            // сжатие памяти
            // переупорядочение свободных блоков памяти
            return sortFreeBlocks(compressAllMemory(currentState));
        }

        Types::MemoryState allocateMemory(
                const Requests::AllocateMemory request,
                const Types::MemoryState& state
        ) const
        {
            return sortFreeBlocks(allocateMemoryGeneral(request, state, false));
        }

        Types::MemoryState freeMemory(
                const Requests::FreeMemory request,
                const Types::MemoryState& state
        ) const
        {
            auto currentState = state;
            auto [blocks, freeBlocks] = currentState;

            // ищем блок, начинающийся с заданного адреса
            auto pos = std::find_if(
                        blocks.begin(),
                        blocks.end(),
                        [&request](const auto& block) {
                return block.address() == request.address;
            });
            // если такого блока нет, игнорируем заявку
            if (pos == blocks.end()) {
                return state;
            }
            // если блок выделен другому процессу, игнорируем заявку
            if (pos->pid() != request.pid) {
                return state;
            }

            // освобождаем блок
            uint32_t index = static_cast<uint32_t>(pos - blocks.begin());
            currentState = Operations::freeMemory(state, request.pid, index);

            // сжатие памяти
            // переупорядочение свободных блоков памяти
            return sortFreeBlocks(compressAllMemory(currentState));
        }

        virtual ~AbstractStrategy()
        {}
    protected:
        virtual Types::MemoryState sortFreeBlocks(
                const Types::MemoryState& state
        ) const = 0;

        virtual std::vector<Types::MemoryBlock>::const_iterator findFreeBlock(
                const std::vector<Types::MemoryBlock>& blocks,
                const std::vector<Types::MemoryBlock>& freeBlocks,
                int32_t size
        ) const
        {
            auto freeBlockPos = std::find_if(
                        freeBlocks.cbegin(),
                        freeBlocks.cend(),
                        [&size](const auto& block) {
                return size < block.size();
            });
            if (freeBlockPos != freeBlocks.cend()) {
                return std::find(blocks.cbegin(), blocks.cend(), *freeBlockPos);
            } else {
                return blocks.cend();
            }
        }

        virtual Types::MemoryState compressAllMemory(
                const Types::MemoryState& state
        ) const
        {
            auto currentState = state;

            while (true) {
                auto [blocks, freeBlocks] = currentState;
                // ищем первый свободный блок памяти
                // проверяем, есть ли за ним хотя бы один свободный блок
                uint32_t index = 0;
                for (; index < blocks.size() - 1 &&
                     !(blocks[index].pid() == -1 &&
                      blocks[index + 1].pid() == -1); ++index) {}

                // если есть, то выполняем сжатие
                if (index < blocks.size() - 1) {
                    currentState = Operations::compressMemory(currentState, index);
                } else {
                    break;
                }
            }

            return currentState;
        }

        virtual Types::MemoryState allocateMemoryGeneral(
                const Requests::AllocateMemory request,
                const Types::MemoryState& state,
                const bool createProcess = false
        ) const
        {
            auto [blocks, freeBlocks] = state;

            // проверить, выделены ли процессу какие-либо блоки памяти
            auto processPos = std::find_if(
                        blocks.begin(),
                        blocks.end(),
                        [&request](const auto& block) {
                return block.pid() == request.pid;
            });
            // если процесс должен существовать, а выделенных для него блоков нет,
            // то заявку проигнорировать
            if (processPos == blocks.end() && !createProcess) {
                return state;
            }

            int32_t totalFree = 0;
            for (const auto& block : freeBlocks) {
                totalFree += block.size();
            }

            // проверить, если свободный блок подходящего размера
            // если есть, то выделить процессу память в этом блоке
            if (auto pos = findFreeBlock(blocks, freeBlocks, request.pages); pos != blocks.end()) {
                uint32_t index = static_cast<uint32_t>(pos - blocks.cbegin());

                return Operations::allocateMemory(state, index, request.pid, request.pages);
            } else if (totalFree > request.pages) { // если суммарно свободной памяти достаточно, то выполнить дефрагментацию
                auto newState = Operations::defragmentMemory(state);
                auto [blocks, freeBlocks] = newState;
                auto pos = findFreeBlock(blocks, freeBlocks, request.pages);
                uint32_t index = static_cast<uint32_t>(pos - blocks.cbegin());

                return Operations::allocateMemory(newState, index, request.pid, request.pages);
            } else { // недостаточно свободной памяти, проигнорировать заявку
                return state;
            }
        }
    };

    using StrategyPtr = std::shared_ptr<AbstractStrategy>;

    class FirstAppropriateStrategy final : public AbstractStrategy {
    public:
        std::string toString() const override
        {
            return "FIRST_APPROPRIATE";
        }

        static std::shared_ptr<FirstAppropriateStrategy> create()
        {
            return std::shared_ptr<FirstAppropriateStrategy>(new FirstAppropriateStrategy());
        }
    protected:
        Types::MemoryState sortFreeBlocks(
                const Types::MemoryState& state
        ) const override
        {
            auto currentState = state;
            auto [blocks, freeBlocks] = currentState;

            // упорядочиваем блоки:
            // - по начальному адресу в порядке возрастания
            std::stable_sort(
                        freeBlocks.begin(),
                        freeBlocks.end(),
                        [](const auto& left, const auto& right) {
                return left.address() < right.address();
            });

            return {blocks, freeBlocks};
        }
    private:
        FirstAppropriateStrategy() :
            AbstractStrategy(StrategyType::FIRST_APPROPRIATE)
        {}
    };

    class MostAppropriateStrategy final : public AbstractStrategy {
    public:
        std::string toString() const override
        {
            return "MOST_APPROPRIATE";
        }

        static std::shared_ptr<MostAppropriateStrategy> create()
        {
            return std::shared_ptr<MostAppropriateStrategy>(new MostAppropriateStrategy());
        }
    private:
        MostAppropriateStrategy() :
            AbstractStrategy(StrategyType::MOST_APPROPRIATE)
        {}
    protected:
        Types::MemoryState sortFreeBlocks(
                const Types::MemoryState& state
        ) const override
        {
            auto currentState = state;
            auto [blocks, freeBlocks] = currentState;

            // упорядочиваем блоки:
            // - по размеру в порядке возрастания
            // - по начальному адресу в порядке возрастания
            std::stable_sort(
                        freeBlocks.begin(),
                        freeBlocks.end(),
                        [](const auto& left, const auto& right) {
                if (left.size() == right.size()) {
                    return left.address() < right.address();
                } else {
                    return left.size() < right.size();
                }
            });

            return {blocks, freeBlocks};
        }
    };

    class LeastAppropriateStrategy final : public AbstractStrategy {
    public:
        std::string toString() const override
        {
            return "LEAST_APPROPRIATE";
        }

        static std::shared_ptr<LeastAppropriateStrategy> create()
        {
            return std::shared_ptr<LeastAppropriateStrategy>(new LeastAppropriateStrategy());
        }
    private:
        LeastAppropriateStrategy() :
            AbstractStrategy(StrategyType::LEAST_APPROPRIATE)
        {}
    protected:
        Types::MemoryState sortFreeBlocks(
                const Types::MemoryState& state
        ) const override
        {
            auto currentState = state;
            auto [blocks, freeBlocks] = currentState;

            // упорядочиваем блоки:
            // - по размеру в порядке убывания
            // - по начальному адресу в порядке возрастания
            std::stable_sort(
                        freeBlocks.begin(),
                        freeBlocks.end(),
                        [](const auto& left, const auto& right) {
                if (left.size() == right.size()) {
                    return left.address() < right.address();
                } else {
                    return left.size() > right.size();
                }
            });

            return {blocks, freeBlocks};
        }
    };
}
}

#endif // STRATEGIES_H
