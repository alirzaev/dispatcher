#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include <vector>

#include "algo/memory/requests.h"
#include "algo/memory/types.h"
#include "algo/memory/operations.h"
#include "algo/memory/strategies.h"
#include "algo/memory/exceptions.h"

using namespace MemoryManagement;

TEST_CASE("test_memory_requests") {
    SECTION("Create instance of CreateProcess")
    {
        auto request = Requests::CreateProcess::create(0, 4096, 1);

        REQUIRE(request->pid == 0);
        REQUIRE(request->bytes == 4096);
        REQUIRE(request->pages == 1);
        REQUIRE(request->type == Requests::RequestType::CREATE_PROCESS);
    }

    SECTION("Convert to JSON instance of CreateProcess")
    {
        auto request = Requests::CreateProcess::create(0, 4096, 1);

        auto expected = nlohmann::json{
            {"type", "CREATE_PROCESS"},
            {"pid", 0},
            {"bytes", 4096},
            {"pages", 1}
        };
        auto actual = request->dump();

        REQUIRE(actual == expected);
    }

    SECTION("Check preconditions for CreateProcess")
    {
        REQUIRE_THROWS_AS(Requests::CreateProcess::create(-2, 4096, 1), Exceptions::RequestException);
        REQUIRE_THROWS_AS(Requests::CreateProcess::create(256, 4096, 1), Exceptions::RequestException);

        REQUIRE_THROWS_AS(Requests::CreateProcess::create(0, -1, 1), Exceptions::RequestException);
        REQUIRE_THROWS_AS(Requests::CreateProcess::create(0, 257 * 4096, 1), Exceptions::RequestException);
        REQUIRE_THROWS_AS(Requests::CreateProcess::create(0, 4097, 1), Exceptions::RequestException);
        REQUIRE_THROWS_AS(Requests::CreateProcess::create(0, 4096, 2), Exceptions::RequestException);

        REQUIRE_THROWS_AS(Requests::CreateProcess::create(0, 4096, 0), Exceptions::RequestException);
        REQUIRE_THROWS_AS(Requests::CreateProcess::create(0, 4096, 257), Exceptions::RequestException);
    }

    SECTION("Copy instance of CreateProcess")
    {
        auto request = Requests::CreateProcess::create(0, 4096, 1);
        auto copy = *request;

        REQUIRE(copy.pid == request->pid);
        REQUIRE(copy.bytes == request->bytes);
        REQUIRE(copy.pages == request->pages);
    }

    SECTION("Create instance of TerminateProcess")
    {
        auto request = Requests::TerminateProcess::create(0);

        REQUIRE(request->pid == 0);
        REQUIRE(request->type == Requests::RequestType::TERMINATE_PROCESS);
    }

    SECTION("Convert to JSON instance of TerminateProcess")
    {
        auto request = Requests::TerminateProcess::create(0);

        auto expected = nlohmann::json{
            {"type", "TERMINATE_PROCESS"},
            {"pid", 0}
        };
        auto actual = request->dump();

        REQUIRE(actual == expected);
    }

    SECTION("Check preconditions for TerminateProcess")
    {
        REQUIRE_THROWS_AS(Requests::TerminateProcess::create(-2), Exceptions::RequestException);
        REQUIRE_THROWS_AS(Requests::TerminateProcess::create(256), Exceptions::RequestException);
    }

    SECTION("Copy instance of TerminateProcess")
    {
        auto request = Requests::TerminateProcess::create(0);
        auto copy = *request;

        REQUIRE(copy.pid == request->pid);
    }

    SECTION("Create instance of AllocateMemory")
    {
        auto request = Requests::AllocateMemory::create(0, 4096, 1);

        REQUIRE(request->pid == 0);
        REQUIRE(request->bytes == 4096);
        REQUIRE(request->pages == 1);
        REQUIRE(request->type == Requests::RequestType::ALLOCATE_MEMORY);
    }

    SECTION("Convert to JSON instance of AllocateMemory")
    {
        auto request = Requests::AllocateMemory::create(0, 4096, 1);

        auto expected = nlohmann::json{
            {"type", "ALLOCATE_MEMORY"},
            {"pid", 0},
            {"bytes", 4096},
            {"pages", 1}
        };
        auto actual = request->dump();

        REQUIRE(actual == expected);
    }

    SECTION("Check preconditions for AllocateMemory")
    {
        REQUIRE_THROWS_AS(Requests::AllocateMemory::create(-2, 4096, 1), Exceptions::RequestException);
        REQUIRE_THROWS_AS(Requests::AllocateMemory::create(256, 4096, 1), Exceptions::RequestException);

        REQUIRE_THROWS_AS(Requests::AllocateMemory::create(0, -1, 1), Exceptions::RequestException);
        REQUIRE_THROWS_AS(Requests::AllocateMemory::create(0, 257 * 4096, 1), Exceptions::RequestException);
        REQUIRE_THROWS_AS(Requests::AllocateMemory::create(0, 4097, 1), Exceptions::RequestException);
        REQUIRE_THROWS_AS(Requests::AllocateMemory::create(0, 4096, 2), Exceptions::RequestException);

        REQUIRE_THROWS_AS(Requests::AllocateMemory::create(0, 4096, 0), Exceptions::RequestException);
        REQUIRE_THROWS_AS(Requests::AllocateMemory::create(0, 4096, 257), Exceptions::RequestException);
    }

    SECTION("Copy instance of AllocateMemory")
    {
        auto request = Requests::AllocateMemory::create(0, 4096, 1);
        auto copy = *request;

        REQUIRE(copy.pid == request->pid);
        REQUIRE(copy.bytes == request->bytes);
        REQUIRE(copy.pages == request->pages);
    }

    SECTION("Create instance of FreeMemory")
    {
        auto request = Requests::FreeMemory::create(0, 12);

        REQUIRE(request->pid == 0);
        REQUIRE(request->address == 12);
        REQUIRE(request->type == Requests::RequestType::FREE_MEMORY);
    }

    SECTION("Convert to JSON instance of FreeMemory")
    {
        auto request = Requests::FreeMemory::create(0, 12);

        auto expected = nlohmann::json{
            {"type", "FREE_MEMORY"},
            {"pid", 0},
            {"address", 12}
        };
        auto actual = request->dump();

        REQUIRE(actual == expected);
    }

    SECTION("Check preconditions for FreeMemory")
    {
        REQUIRE_THROWS_AS(Requests::FreeMemory::create(-2, 0), Exceptions::RequestException);
        REQUIRE_THROWS_AS(Requests::FreeMemory::create(256, 0), Exceptions::RequestException);

        REQUIRE_THROWS_AS(Requests::FreeMemory::create(0, -1), Exceptions::RequestException);
        REQUIRE_THROWS_AS(Requests::FreeMemory::create(0, 256), Exceptions::RequestException);
    }

    SECTION("Copy instance of FreeMemory")
    {
        auto request = Requests::FreeMemory::create(0, 12);
        auto copy = *request;

        REQUIRE(copy.pid == request->pid);
        REQUIRE(copy.address == request->address);
    }

    SECTION("Create array of AbstractRequests")
    {
        Requests::RequestPtr requests[] = {
            Requests::CreateProcess::create(0, 2, 1),
            Requests::TerminateProcess::create(0)
        };

        REQUIRE(requests[0]->type == Requests::RequestType::CREATE_PROCESS);
        REQUIRE(requests[1]->type == Requests::RequestType::TERMINATE_PROCESS);
    }

    SECTION("Dynamic cast from AbstractRequest to CreateProcess")
    {
        Requests::RequestPtr request = Requests::CreateProcess::create(0, 2, 1);
        auto* createProcess = dynamic_cast<Requests::CreateProcess*>(request.get());
        REQUIRE(createProcess != nullptr);
        REQUIRE(createProcess->type == Requests::RequestType::CREATE_PROCESS);
    }
}

TEST_CASE("test_memory_operations") {
    SECTION("Memory allocation")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Memory allocation (entire block)")
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
            MemoryBlock{3, 15, 20},
            MemoryBlock{2, 35, 1},
            MemoryBlock{-1, 36, 7}
        };
        std::vector<Types::MemoryBlock> expectedFreeBlocks = {
            MemoryBlock{-1, 36, 7}
        };
        Types::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

        auto actualState = Operations::allocateMemory(state, 2, 3, 20);
        REQUIRE(actualState == expectedState);
    }

    SECTION("Memory deallocation")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Memory defragmentation")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Memory compressing")
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
        REQUIRE(actualState == expectedState);
    }
}

TEST_CASE("test_memory_first_appropriate_strategy") {
    SECTION("Create instance of FirstAppropriateStrategy")
    {
        auto strategy = Strategies::FirstAppropriateStrategy::create();

        REQUIRE(strategy->type == Strategies::StrategyType::FIRST_APPROPRIATE);
    }

    SECTION("Process CreateProcess request")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process TerminateProcess request")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process AllocateMemory request")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process AllocateMemory request (with defragmentation)")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process AllocateMemory request (out of memory)")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process FreeMemory request")
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
        REQUIRE(actualState == expectedState);
    }
}

TEST_CASE("test_memory_most_appropriate_strategy") {
    SECTION("Create instance of MostAppropriateStrategy")
    {
        auto strategy = Strategies::MostAppropriateStrategy::create();

        REQUIRE(strategy->type == Strategies::StrategyType::MOST_APPROPRIATE);
    }

    SECTION("Process CreateProcess request")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process TerminateProcess request")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process AllocateMemory request")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process AllocateMemory request (with defragmentation)")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process AllocateMemory request (out of memory)")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process FreeMemory request")
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
        REQUIRE(actualState == expectedState);
    }
}

TEST_CASE("test_memory_least_appropriate_strategy") {
    SECTION("Create instance of LeastAppropriateStrategy")
    {
        auto strategy = Strategies::LeastAppropriateStrategy::create();

        REQUIRE(strategy->type == Strategies::StrategyType::LEAST_APPROPRIATE);
    }

    SECTION("Process CreateProcess request")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process TerminateProcess request")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process AllocateMemory request")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process AllocateMemory request (with defragmentation)")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process AllocateMemory request (out of memory)")
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
        REQUIRE(actualState == expectedState);
    }

    SECTION("Process FreeMemory request")
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
        REQUIRE(actualState == expectedState);
    }
}
