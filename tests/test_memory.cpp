#include "lest/lest_basic.hpp"

#include <vector>

#include "../schedulers/algo/memory/requests.h"
#include "../schedulers/algo/memory/types.h"
#include "../schedulers/algo/memory/operations.h"
#include "../schedulers/algo/memory/strategies.h"

using namespace MemoryManagement;

const lest::test test_memory_requests[] = {
    {
        CASE("Create instance of CreateProcess")
        {
            auto request = Requests::CreateProcess::create(0, 4096, 1);

            EXPECT(request->pid == 0);
            EXPECT(request->bytes == 4096);
            EXPECT(request->pages == 1);
            EXPECT(request->type == Requests::RequestType::CREATE_PROCESS);
        }
    },
    {
        CASE("Copy instance of CreateProcess")
        {
            auto request = Requests::CreateProcess::create(0, 4096, 1);
            auto copy = *request;

            EXPECT(copy.pid == request->pid);
            EXPECT(copy.bytes == request->bytes);
            EXPECT(copy.pages == request->pages);
        }
    },
    {
        CASE("Create instance of TerminateProcess")
        {
            auto request = Requests::TerminateProcess::create(0);

            EXPECT(request->pid == 0);
            EXPECT(request->type == Requests::RequestType::TERMINATE_PROCESS);
        }
    },
    {
        CASE("Copy instance of TerminateProcess")
        {
            auto request = Requests::TerminateProcess::create(0);
            auto copy = *request;

            EXPECT(copy.pid == request->pid);
        }
    },
    {
        CASE("Create instance of AllocateMemory")
        {
            auto request = Requests::AllocateMemory::create(0, 4096, 1);

            EXPECT(request->pid == 0);
            EXPECT(request->bytes == 4096);
            EXPECT(request->pages == 1);
            EXPECT(request->type == Requests::RequestType::ALLOCATE_MEMORY);
        }
    },
    {
        CASE("Copy instance of AllocateMemory")
        {
            auto request = Requests::AllocateMemory::create(0, 4096, 1);
            auto copy = *request;

            EXPECT(copy.pid == request->pid);
            EXPECT(copy.bytes == request->bytes);
            EXPECT(copy.pages == request->pages);
        }
    },
    {
        CASE("Create instance of FreeMemory")
        {
            auto request = Requests::FreeMemory::create(0, 12);

            EXPECT(request->pid == 0);
            EXPECT(request->address == 12);
            EXPECT(request->type == Requests::RequestType::FREE_MEMORY);
        }
    },
    {
        CASE("Copy instance of FreeMemory")
        {
            auto request = Requests::FreeMemory::create(0, 12);
            auto copy = *request;

            EXPECT(copy.pid == request->pid);
            EXPECT(copy.address == request->address);
        }
    },
    {
        CASE("Create array of AbstractRequests")
        {
            Requests::RequestPtr requests[] = {
                Requests::CreateProcess::create(0, 2, 1),
                Requests::TerminateProcess::create(0)
            };

            EXPECT(requests[0]->type == Requests::RequestType::CREATE_PROCESS);
            EXPECT(requests[1]->type == Requests::RequestType::TERMINATE_PROCESS);
        }
    },
    {
        CASE("Dynamic cast from AbstractRequest to CreateProcess")
        {
            Requests::RequestPtr request = Requests::CreateProcess::create(0, 2, 1);
            auto* createProcess = dynamic_cast<Requests::CreateProcess*>(request.get());
            EXPECT(createProcess != nullptr);
            EXPECT(createProcess->type == Requests::RequestType::CREATE_PROCESS);
        }
    }
};

const lest::test test_memory_operations[] = {
    {
        CASE("Memory allocation")
        {
            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{0, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20}, //*
                MemoryBlock{2, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 15, 20},
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{0, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{3, 15, 4},
                MemoryBlock{-1, 19, 16},
                MemoryBlock{2, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 36, 7},
                MemoryBlock{-1, 19, 16}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = Operations::allocateMemory(state, 2, 3, 4);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Memory deallocation")
        {
            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{0, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{2, 35, 1}, //*
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 15, 20},
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{0, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{-1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 15, 20},
                MemoryBlock{-1, 36, 7},
                MemoryBlock{-1, 35, 1}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = Operations::freeMemory(state, 2, 3);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Memory defragmentation")
        {
            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{0, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20}, //*
                MemoryBlock{2, 35, 1},
                MemoryBlock{-1, 36, 7} //*
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 15, 20},
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{0, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{2, 15, 1},
                MemoryBlock{-1, 16, 27}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 16, 27}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = Operations::defragmentMemory(state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Memory compressing")
        {
            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{0, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20}, //*
                MemoryBlock{-1, 35, 1},  //*
                MemoryBlock{-1, 36, 7},  //*
                MemoryBlock{4, 43, 12},
                MemoryBlock{-1, 55, 2},
                MemoryBlock{-1, 57, 6}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 15, 20},
                MemoryBlock{-1, 35, 1},
                MemoryBlock{-1, 36, 7},
                MemoryBlock{-1, 55, 2},
                MemoryBlock{-1, 57, 6}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{0, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 28},
                MemoryBlock{4, 43, 12},
                MemoryBlock{-1, 55, 2},
                MemoryBlock{-1, 57, 6}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 55, 2},
                MemoryBlock{-1, 57, 6},
                MemoryBlock{-1, 15, 28}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = Operations::compressMemory(state, 2);
            EXPECT(actualState == expectedState);
        }
    }
};

const lest::test test_memory_strategies[] = {
    {
        CASE("Create instance of FirstAppropriateStrategy")
        {
            auto strategy = Strategies::FirstAppropriateStrategy::create();

            EXPECT(strategy->type == Strategies::StrategyType::FIRST_APPROPRIATE);
        }
    },
    {
        CASE("Process CreateProcess request")
        {
            auto strategy = Strategies::FirstAppropriateStrategy::create();
            auto request = *Requests::CreateProcess::create(3, 16384, 4);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{0, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20}, //*
                MemoryBlock{2, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 15, 20},
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{0, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{3, 15, 4},
                MemoryBlock{-1, 19, 16},
                MemoryBlock{2, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 36, 7},
                MemoryBlock{-1, 19, 16}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->createProcess(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Create instance of MostAppropriateStrategy")
        {
            auto strategy = Strategies::MostAppropriateStrategy::create();

            EXPECT(strategy->type == Strategies::StrategyType::MOST_APPROPRIATE);
        }
    },
    {
        CASE("Create instance of LeastAppropriateStrategy")
        {
            auto strategy = Strategies::LeastAppropriateStrategy::create();

            EXPECT(strategy->type == Strategies::StrategyType::LEAST_APPROPRIATE);
        }
    }
};
