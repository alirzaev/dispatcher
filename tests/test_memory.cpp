#include "../libs/lest/lest_basic.hpp"
#include "../libs/nlohmann/json.hpp"

#include <vector>

#include "../schedulers/algo/memory/requests.h"
#include "../schedulers/algo/memory/types.h"
#include "../schedulers/algo/memory/operations.h"
#include "../schedulers/algo/memory/strategies.h"
#include "../schedulers/algo/memory/exceptions.h"

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
        CASE("Convert to JSON instance of CreateProcess")
        {
            auto request = Requests::CreateProcess::create(0, 4096, 1);

            auto expected = nlohmann::json{
                {"type", "CREATE_PROCESS"},
                {"pid", 0},
                {"bytes", 4096},
                {"pages", 1}
            };
            auto actual = request->dump();

            EXPECT(actual == expected);
        }
    },
    {
        CASE("Check preconditions for CreateProcess")
        {
            EXPECT_THROWS_AS(Requests::CreateProcess::create(-2, 4096, 1), Exceptions::RequestException);
            EXPECT_THROWS_AS(Requests::CreateProcess::create(256, 4096, 1), Exceptions::RequestException);

            EXPECT_THROWS_AS(Requests::CreateProcess::create(0, -1, 1), Exceptions::RequestException);
            EXPECT_THROWS_AS(Requests::CreateProcess::create(0, 257 * 4096, 1), Exceptions::RequestException);
            EXPECT_THROWS_AS(Requests::CreateProcess::create(0, 4097, 1), Exceptions::RequestException);
            EXPECT_THROWS_AS(Requests::CreateProcess::create(0, 4096, 2), Exceptions::RequestException);

            EXPECT_THROWS_AS(Requests::CreateProcess::create(0, 4096, 0), Exceptions::RequestException);
            EXPECT_THROWS_AS(Requests::CreateProcess::create(0, 4096, 257), Exceptions::RequestException);
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
        CASE("Convert to JSON instance of TerminateProcess")
        {
            auto request = Requests::TerminateProcess::create(0);

            auto expected = nlohmann::json{
                {"type", "TERMINATE_PROCESS"},
                {"pid", 0}
            };
            auto actual = request->dump();

            EXPECT(actual == expected);
        }
    },
    {
        CASE("Check preconditions for TerminateProcess")
        {
            EXPECT_THROWS_AS(Requests::TerminateProcess::create(-2), Exceptions::RequestException);
            EXPECT_THROWS_AS(Requests::TerminateProcess::create(256), Exceptions::RequestException);
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
        CASE("Convert to JSON instance of AllocateMemory")
        {
            auto request = Requests::AllocateMemory::create(0, 4096, 1);

            auto expected = nlohmann::json{
                {"type", "ALLOCATE_MEMORY"},
                {"pid", 0},
                {"bytes", 4096},
                {"pages", 1}
            };
            auto actual = request->dump();

            EXPECT(actual == expected);
        }
    },
    {
        CASE("Check preconditions for AllocateMemory")
        {
            EXPECT_THROWS_AS(Requests::AllocateMemory::create(-2, 4096, 1), Exceptions::RequestException);
            EXPECT_THROWS_AS(Requests::AllocateMemory::create(256, 4096, 1), Exceptions::RequestException);

            EXPECT_THROWS_AS(Requests::AllocateMemory::create(0, -1, 1), Exceptions::RequestException);
            EXPECT_THROWS_AS(Requests::AllocateMemory::create(0, 257 * 4096, 1), Exceptions::RequestException);
            EXPECT_THROWS_AS(Requests::AllocateMemory::create(0, 4097, 1), Exceptions::RequestException);
            EXPECT_THROWS_AS(Requests::AllocateMemory::create(0, 4096, 2), Exceptions::RequestException);

            EXPECT_THROWS_AS(Requests::AllocateMemory::create(0, 4096, 0), Exceptions::RequestException);
            EXPECT_THROWS_AS(Requests::AllocateMemory::create(0, 4096, 257), Exceptions::RequestException);
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
        CASE("Convert to JSON instance of FreeMemory")
        {
            auto request = Requests::FreeMemory::create(0, 12);

            auto expected = nlohmann::json{
                {"type", "FREE_MEMORY"},
                {"pid", 0},
                {"address", 12}
            };
            auto actual = request->dump();

            EXPECT(actual == expected);
        }
    },
    {
        CASE("Check preconditions for FreeMemory")
        {
            EXPECT_THROWS_AS(Requests::FreeMemory::create(-2, 0), Exceptions::RequestException);
            EXPECT_THROWS_AS(Requests::FreeMemory::create(256, 0), Exceptions::RequestException);

            EXPECT_THROWS_AS(Requests::FreeMemory::create(0, -1), Exceptions::RequestException);
            EXPECT_THROWS_AS(Requests::FreeMemory::create(0, 256), Exceptions::RequestException);
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

const lest::test test_memory_first_appropriate_strategy[] = {
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
                MemoryBlock{-1, 19, 16},
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->createProcess(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process TerminateProcess request")
        {
            auto strategy = Strategies::FirstAppropriateStrategy::create();
            auto request = *Requests::TerminateProcess::create(2);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{2, 0, 12},  //*
                MemoryBlock{2, 12, 3},  //*
                MemoryBlock{2, 15, 20}, //*
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{-1, 0, 35},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 0, 35},
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->terminateProcess(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process AllocateMemory request")
        {
            auto strategy = Strategies::FirstAppropriateStrategy::create();
            auto request = *Requests::AllocateMemory::create(1, 8192, 2);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 15, 20}, //*
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{1, 15, 2},
                MemoryBlock{-1, 17, 18},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 17, 18},
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->allocateMemory(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process AllocateMemory request (with defragmentation)")
        {
            auto strategy = Strategies::FirstAppropriateStrategy::create();
            auto request = *Requests::AllocateMemory::create(1, 90112, 22);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 15, 20}, //*
                MemoryBlock{-1, 36, 7} //*
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{1, 15, 1},
                MemoryBlock{1, 16, 22},
                MemoryBlock{-1, 38, 5}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 38, 5}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->allocateMemory(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process AllocateMemory request (out of memory)")
        {
            auto strategy = Strategies::FirstAppropriateStrategy::create();
            auto request = *Requests::AllocateMemory::create(1, 122800, 30);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 15, 20},
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 15, 20},
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->allocateMemory(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process FreeMemory request")
        {
            auto strategy = Strategies::FirstAppropriateStrategy::create();
            auto request = *Requests::FreeMemory::create(1, 35);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{2, 15, 20},
                MemoryBlock{1, 35, 1}, //*
                MemoryBlock{-1, 36, 7} //*
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{2, 15, 20},
                MemoryBlock{-1, 35, 8}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 35, 8}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->freeMemory(request, state);
            EXPECT(actualState == expectedState);
        }
    }
};

const lest::test test_memory_most_appropriate_strategy[] = {
    {
        CASE("Create instance of MostAppropriateStrategy")
        {
            auto strategy = Strategies::MostAppropriateStrategy::create();

            EXPECT(strategy->type == Strategies::StrategyType::MOST_APPROPRIATE);
        }
    },
    {
        CASE("Process CreateProcess request")
        {
            auto strategy = Strategies::MostAppropriateStrategy::create();
            auto request = *Requests::CreateProcess::create(3, 16384, 4);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{0, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{2, 35, 1},
                MemoryBlock{-1, 36, 7} //*
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 36, 7},
                MemoryBlock{-1, 15, 20}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{0, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{2, 35, 1},
                MemoryBlock{3, 36, 4},
                MemoryBlock{-1, 40, 3}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 40, 3},
                MemoryBlock{-1, 15, 20}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->createProcess(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process TerminateProcess request")
        {
            auto strategy = Strategies::MostAppropriateStrategy::create();
            auto request = *Requests::TerminateProcess::create(2);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{2, 0, 12},  //*
                MemoryBlock{2, 12, 3},  //*
                MemoryBlock{2, 15, 20}, //*
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{-1, 0, 35},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 36, 7},
                MemoryBlock{-1, 0, 35}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->terminateProcess(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process AllocateMemory request")
        {
            auto strategy = Strategies::MostAppropriateStrategy::create();
            auto request = *Requests::AllocateMemory::create(1, 8192, 2);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 36, 7}, //*
                MemoryBlock{-1, 15, 20}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{1, 35, 1},
                MemoryBlock{1, 36, 2},
                MemoryBlock{-1, 38, 5}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 38, 5},
                MemoryBlock{-1, 15, 20}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->allocateMemory(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process AllocateMemory request (with defragmentation)")
        {
            auto strategy = Strategies::MostAppropriateStrategy::create();
            auto request = *Requests::AllocateMemory::create(1, 90112, 22);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 36, 7}, //*
                MemoryBlock{-1, 15, 20} //*
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{1, 15, 1},
                MemoryBlock{1, 16, 22},
                MemoryBlock{-1, 38, 5}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 38, 5}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->allocateMemory(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process AllocateMemory request (out of memory)")
        {
            auto strategy = Strategies::MostAppropriateStrategy::create();
            auto request = *Requests::AllocateMemory::create(1, 122800, 30);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 36, 7},
                MemoryBlock{-1, 15, 20}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 36, 7},
                MemoryBlock{-1, 15, 20}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->allocateMemory(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process FreeMemory request")
        {
            auto strategy = Strategies::MostAppropriateStrategy::create();
            auto request = *Requests::FreeMemory::create(1, 35);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{2, 15, 20},
                MemoryBlock{1, 35, 1}, //*
                MemoryBlock{-1, 36, 7} //*
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{2, 15, 20},
                MemoryBlock{-1, 35, 8}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 35, 8}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->freeMemory(request, state);
            EXPECT(actualState == expectedState);
        }
    }
};

const lest::test test_memory_least_appropriate_strategy[] = {
    {
        CASE("Create instance of LeastAppropriateStrategy")
        {
            auto strategy = Strategies::LeastAppropriateStrategy::create();

            EXPECT(strategy->type == Strategies::StrategyType::LEAST_APPROPRIATE);
        }
    },
    {
        CASE("Process CreateProcess request")
        {
            auto strategy = Strategies::LeastAppropriateStrategy::create();
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
                MemoryBlock{-1, 19, 16},
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->createProcess(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process TerminateProcess request")
        {
            auto strategy = Strategies::LeastAppropriateStrategy::create();
            auto request = *Requests::TerminateProcess::create(2);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{-1, 0, 7},
                MemoryBlock{1, 7, 1},
                MemoryBlock{2, 8, 12},  //*
                MemoryBlock{2, 20, 3},  //*
                MemoryBlock{2, 23, 20}, //*
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 0, 7}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{-1, 0, 7},
                MemoryBlock{1, 7, 1},
                MemoryBlock{-1, 8, 35}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 8, 35},
                MemoryBlock{-1, 0, 7}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->terminateProcess(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process AllocateMemory request")
        {
            auto strategy = Strategies::LeastAppropriateStrategy::create();
            auto request = *Requests::AllocateMemory::create(1, 8192, 2);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 15, 20}, //*
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{1, 15, 2},
                MemoryBlock{-1, 17, 18},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 17, 18},
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->allocateMemory(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process AllocateMemory request (with defragmentation)")
        {
            auto strategy = Strategies::LeastAppropriateStrategy::create();
            auto request = *Requests::AllocateMemory::create(1, 90112, 22);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 15, 20}, //*
                MemoryBlock{-1, 36, 7} //*
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{1, 15, 1},
                MemoryBlock{1, 16, 22},
                MemoryBlock{-1, 38, 5}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 38, 5}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->allocateMemory(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process AllocateMemory request (out of memory)")
        {
            auto strategy = Strategies::LeastAppropriateStrategy::create();
            auto request = *Requests::AllocateMemory::create(1, 122800, 30);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 15, 20},
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{-1, 15, 20},
                MemoryBlock{1, 35, 1},
                MemoryBlock{-1, 36, 7}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 15, 20},
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->allocateMemory(request, state);
            EXPECT(actualState == expectedState);
        }
    },
    {
        CASE("Process FreeMemory request")
        {
            auto strategy = Strategies::LeastAppropriateStrategy::create();
            auto request = *Requests::FreeMemory::create(1, 35);

            std::vector<Types::MemoryBlock> blocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{2, 15, 20},
                MemoryBlock{1, 35, 1}, //*
                MemoryBlock{-1, 36, 7} //*
            };
            std::vector<Types::MemoryBlock> freeBlocks = {
                MemoryBlock{-1, 36, 7}
            };
            Types::MemoryState state(blocks, freeBlocks);

            std::vector<Types::MemoryBlock> expectedBlocks = {
                MemoryBlock{2, 0, 12},
                MemoryBlock{2, 12, 3},
                MemoryBlock{2, 15, 20},
                MemoryBlock{-1, 35, 8}
            };
            std::vector<Types::MemoryBlock> expectedFreeBlocks = {
                MemoryBlock{-1, 35, 8}
            };
            Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

            auto actualState = strategy->freeMemory(request, state);
            EXPECT(actualState == expectedState);
        }
    }
};
