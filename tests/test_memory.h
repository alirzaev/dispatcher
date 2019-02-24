#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include <vector>

#include "algo/memory/exceptions.h"
#include "algo/memory/operations.h"
#include "algo/memory/requests.h"
#include "algo/memory/strategies.h"
#include "algo/memory/types.h"

using namespace MemoryManagement;
using std::vector;

TEST_CASE("test_memory_requests") {
  SECTION("Create instance of CreateProcessReq") {
    auto request = CreateProcessReq(0, 4096, 1);

    REQUIRE(request.pid() == 0);
    REQUIRE(request.bytes() == 4096);
    REQUIRE(request.pages() == 1);
  }

  SECTION("Convert to JSON instance of CreateProcessReq") {
    auto request = CreateProcessReq(0, 4096, 1);

    auto expected = nlohmann::json{
        {"type", "CREATE_PROCESS"}, {"pid", 0}, {"bytes", 4096}, {"pages", 1}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Check preconditions for CreateProcessReq") {
    REQUIRE_THROWS_AS(CreateProcessReq(-2, 4096, 1), RequestException);
    REQUIRE_THROWS_AS(CreateProcessReq(256, 4096, 1), RequestException);

    REQUIRE_THROWS_AS(CreateProcessReq(0, -1, 1), RequestException);
    REQUIRE_THROWS_AS(CreateProcessReq(0, 257 * 4096, 1), RequestException);
    REQUIRE_THROWS_AS(CreateProcessReq(0, 4097, 1), RequestException);
    REQUIRE_THROWS_AS(CreateProcessReq(0, 4096, 2), RequestException);

    REQUIRE_THROWS_AS(CreateProcessReq(0, 4096, 0), RequestException);
    REQUIRE_THROWS_AS(CreateProcessReq(0, 4096, 257), RequestException);
  }

  SECTION("Create instance of TerminateProcessReq") {
    auto request = TerminateProcessReq(0);

    REQUIRE(request.pid() == 0);
  }

  SECTION("Convert to JSON instance of TerminateProcessReq") {
    auto request = TerminateProcessReq(0);

    auto expected = nlohmann::json{{"type", "TERMINATE_PROCESS"}, {"pid", 0}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Check preconditions for TerminateProcessReq") {
    REQUIRE_THROWS_AS(TerminateProcessReq(-2), RequestException);
    REQUIRE_THROWS_AS(TerminateProcessReq(256), RequestException);
  }

  SECTION("Create instance of AllocateMemory") {
    auto request = AllocateMemory(0, 4096, 1);

    REQUIRE(request.pid() == 0);
    REQUIRE(request.bytes() == 4096);
    REQUIRE(request.pages() == 1);
  }

  SECTION("Convert to JSON instance of AllocateMemory") {
    auto request = AllocateMemory(0, 4096, 1);

    auto expected = nlohmann::json{
        {"type", "ALLOCATE_MEMORY"}, {"pid", 0}, {"bytes", 4096}, {"pages", 1}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Check preconditions for AllocateMemory") {
    REQUIRE_THROWS_AS(AllocateMemory(-2, 4096, 1), RequestException);
    REQUIRE_THROWS_AS(AllocateMemory(256, 4096, 1), RequestException);

    REQUIRE_THROWS_AS(AllocateMemory(0, -1, 1), RequestException);
    REQUIRE_THROWS_AS(AllocateMemory(0, 257 * 4096, 1), RequestException);
    REQUIRE_THROWS_AS(AllocateMemory(0, 4097, 1), RequestException);
    REQUIRE_THROWS_AS(AllocateMemory(0, 4096, 2), RequestException);

    REQUIRE_THROWS_AS(AllocateMemory(0, 4096, 0), RequestException);
    REQUIRE_THROWS_AS(AllocateMemory(0, 4096, 257), RequestException);
  }

  SECTION("Copy instance of AllocateMemory") {
    auto request = AllocateMemory(0, 4096, 1);
    auto copy = request;

    REQUIRE(copy.pid() == request.pid());
    REQUIRE(copy.bytes() == request.bytes());
    REQUIRE(copy.pages() == request.pages());
  }

  SECTION("Create instance of FreeMemory") {
    auto request = FreeMemory(0, 12);

    REQUIRE(request.pid() == 0);
    REQUIRE(request.address() == 12);
  }

  SECTION("Convert to JSON instance of FreeMemory") {
    auto request = FreeMemory(0, 12);

    auto expected =
        nlohmann::json{{"type", "FREE_MEMORY"}, {"pid", 0}, {"address", 12}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Check preconditions for FreeMemory") {
    REQUIRE_THROWS_AS(FreeMemory(-2, 0), RequestException);
    REQUIRE_THROWS_AS(FreeMemory(256, 0), RequestException);

    REQUIRE_THROWS_AS(FreeMemory(0, -1), RequestException);
    REQUIRE_THROWS_AS(FreeMemory(0, 256), RequestException);
  }
}

TEST_CASE("test_memory_operations") {
  SECTION("Memory allocation") {
    vector<MemoryBlock> blocks = {{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //*
                                  {2, 35, 1},   //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                      {-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{0, 0, 12},   //
                                          {2, 12, 3},   //
                                          {3, 15, 4},   //
                                          {-1, 19, 16}, //
                                          {2, 35, 1},   //
                                          {-1, 36, 7}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 36, 7}, //
                                              {-1, 19, 16}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = allocateMemory(state, 2, 3, 4);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Memory allocation (entire block)") {
    vector<MemoryBlock> blocks = {{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //*
                                  {2, 35, 1},   //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                      {-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{0, 0, 12},  //
                                          {2, 12, 3},  //
                                          {3, 15, 20}, //
                                          {2, 35, 1},  //
                                          {-1, 36, 7}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 36, 7}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = allocateMemory(state, 2, 3, 20);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Memory deallocation") {
    vector<MemoryBlock> blocks = {{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {2, 35, 1},   //*
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                      {-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{0, 0, 12},   //
                                          {2, 12, 3},   //
                                          {-1, 15, 20}, //
                                          {-1, 35, 1},  //
                                          {-1, 36, 7}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 15, 20}, //
                                              {-1, 36, 7},  //
                                              {-1, 35, 1}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = freeMemory(state, 2, 3);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Memory defragmentation") {
    vector<MemoryBlock> blocks = {
        {0, 0, 12},   //
        {2, 12, 3},   //
        {-1, 15, 20}, //*
        {2, 35, 1},   //
        {-1, 36, 7}   //*
    };
    vector<MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                      {-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{0, 0, 12}, //
                                          {2, 12, 3}, //
                                          {2, 15, 1}, //
                                          {-1, 16, 27}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 16, 27}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = defragmentMemory(state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Memory compressing") {
    vector<MemoryBlock> blocks = {{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //*
                                  {-1, 35, 1},  //*
                                  {-1, 36, 7},  //*
                                  {4, 43, 12},  //
                                  {-1, 55, 2},  //
                                  {-1, 57, 6}};
    vector<MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                      {-1, 35, 1},  //
                                      {-1, 36, 7},  //
                                      {-1, 55, 2},  //
                                      {-1, 57, 6}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{0, 0, 12},   //
                                          {2, 12, 3},   //
                                          {-1, 15, 28}, //
                                          {4, 43, 12},  //
                                          {-1, 55, 2},  //
                                          {-1, 57, 6}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 55, 2}, //
                                              {-1, 57, 6}, //
                                              {-1, 15, 28}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = compressMemory(state, 2);
    REQUIRE(actualState == expectedState);
  }
}

TEST_CASE("test_memory_first_appropriate_strategy") {
  SECTION("Create instance of FirstAppropriateStrategy") {
    auto strategy = FirstAppropriateStrategy::create();

    REQUIRE(strategy->type == StrategyType::FIRST_APPROPRIATE);
  }

  SECTION("Process CreateProcessReq request") {
    auto strategy = FirstAppropriateStrategy::create();
    auto request = CreateProcessReq(3, 16384, 4);

    vector<MemoryBlock> blocks = {{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //*
                                  {2, 35, 1},   //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                      {-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{0, 0, 12},   //
                                          {2, 12, 3},   //
                                          {3, 15, 4},   //
                                          {-1, 19, 16}, //
                                          {2, 35, 1},   //
                                          {-1, 36, 7}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 19, 16}, //
                                              {-1, 36, 7}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process TerminateProcessReq request") {
    auto strategy = FirstAppropriateStrategy::create();
    auto request = TerminateProcessReq(2);

    vector<MemoryBlock> blocks = {{2, 0, 12},  //*
                                  {2, 12, 3},  //*
                                  {2, 15, 20}, //*
                                  {1, 35, 1},  //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {{-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{-1, 0, 35}, //
                                          {1, 35, 1},  //
                                          {-1, 36, 7}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 0, 35}, //
                                              {-1, 36, 7}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process AllocateMemory request") {
    auto strategy = FirstAppropriateStrategy::create();
    auto request = AllocateMemory(1, 8192, 2);

    vector<MemoryBlock> blocks = {{2, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {1, 35, 1},   //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {{-1, 15, 20}, //*
                                      {-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{2, 0, 12},   //
                                          {2, 12, 3},   //
                                          {1, 15, 2},   //
                                          {-1, 17, 18}, //
                                          {1, 35, 1},   //
                                          {-1, 36, 7}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 17, 18}, //
                                              {-1, 36, 7}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process AllocateMemory request (with defragmentation)") {
    auto strategy = FirstAppropriateStrategy::create();
    auto request = AllocateMemory(1, 90112, 22);

    vector<MemoryBlock> blocks = {{2, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {1, 35, 1},   //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {
        {-1, 15, 20}, //*
        {-1, 36, 7}   //*
    };
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{2, 0, 12},  //
                                          {2, 12, 3},  //
                                          {1, 15, 1},  //
                                          {1, 16, 22}, //
                                          {-1, 38, 5}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 38, 5}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process AllocateMemory request (out of memory)") {
    auto strategy = FirstAppropriateStrategy::create();
    auto request = AllocateMemory(1, 122800, 30);

    vector<MemoryBlock> blocks = {{2, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {1, 35, 1},   //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                      {-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{2, 0, 12},   //
                                          {2, 12, 3},   //
                                          {-1, 15, 20}, //
                                          {1, 35, 1},   //
                                          {-1, 36, 7}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 15, 20}, //
                                              {-1, 36, 7}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process FreeMemory request") {
    auto strategy = FirstAppropriateStrategy::create();
    auto request = FreeMemory(1, 35);

    vector<MemoryBlock> blocks = {
        {2, 0, 12},  //
        {2, 12, 3},  //
        {2, 15, 20}, //
        {1, 35, 1},  //*
        {-1, 36, 7}  //*
    };
    vector<MemoryBlock> freeBlocks = {{-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{2, 0, 12},  //
                                          {2, 12, 3},  //
                                          {2, 15, 20}, //
                                          {-1, 35, 8}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 35, 8}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }
}

TEST_CASE("test_memory_most_appropriate_strategy") {
  SECTION("Create instance of MostAppropriateStrategy") {
    auto strategy = MostAppropriateStrategy::create();

    REQUIRE(strategy->type == StrategyType::MOST_APPROPRIATE);
  }

  SECTION("Process CreateProcessReq request") {
    auto strategy = MostAppropriateStrategy::create();
    auto request = CreateProcessReq(3, 16384, 4);

    vector<MemoryBlock> blocks = {
        {0, 0, 12},   //
        {2, 12, 3},   //
        {-1, 15, 20}, //
        {2, 35, 1},   //
        {-1, 36, 7}   //*
    };
    vector<MemoryBlock> freeBlocks = {{-1, 36, 7}, //
                                      {-1, 15, 20}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{0, 0, 12},   //
                                          {2, 12, 3},   //
                                          {-1, 15, 20}, //
                                          {2, 35, 1},   //
                                          {3, 36, 4},   //
                                          {-1, 40, 3}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 40, 3}, //
                                              {-1, 15, 20}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process TerminateProcessReq request") {
    auto strategy = MostAppropriateStrategy::create();
    auto request = TerminateProcessReq(2);

    vector<MemoryBlock> blocks = {{2, 0, 12},  //*
                                  {2, 12, 3},  //*
                                  {2, 15, 20}, //*
                                  {1, 35, 1},  //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {{-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{-1, 0, 35}, //
                                          {1, 35, 1},  //
                                          {-1, 36, 7}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 36, 7}, //
                                              {-1, 0, 35}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process AllocateMemory request") {
    auto strategy = MostAppropriateStrategy::create();
    auto request = AllocateMemory(1, 8192, 2);

    vector<MemoryBlock> blocks = {{2, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {1, 35, 1},   //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {{-1, 36, 7}, //*
                                      {-1, 15, 20}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{2, 0, 12},   //
                                          {2, 12, 3},   //
                                          {-1, 15, 20}, //
                                          {1, 35, 1},   //
                                          {1, 36, 2},   //
                                          {-1, 38, 5}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 38, 5}, //
                                              {-1, 15, 20}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process AllocateMemory request (with defragmentation)") {
    auto strategy = MostAppropriateStrategy::create();
    auto request = AllocateMemory(1, 90112, 22);

    vector<MemoryBlock> blocks = {{2, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {1, 35, 1},   //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {
        {-1, 36, 7}, //*
        {-1, 15, 20} //*
    };
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{2, 0, 12},  //
                                          {2, 12, 3},  //
                                          {1, 15, 1},  //
                                          {1, 16, 22}, //
                                          {-1, 38, 5}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 38, 5}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process AllocateMemory request (out of memory)") {
    auto strategy = MostAppropriateStrategy::create();
    auto request = AllocateMemory(1, 122800, 30);

    vector<MemoryBlock> blocks = {{2, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {1, 35, 1},   //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {{-1, 36, 7}, //
                                      {-1, 15, 20}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{2, 0, 12},   //
                                          {2, 12, 3},   //
                                          {-1, 15, 20}, //
                                          {1, 35, 1},   //
                                          {-1, 36, 7}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 36, 7}, //
                                              {-1, 15, 20}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process FreeMemory request") {
    auto strategy = MostAppropriateStrategy::create();
    auto request = FreeMemory(1, 35);

    vector<MemoryBlock> blocks = {
        {2, 0, 12},  //
        {2, 12, 3},  //
        {2, 15, 20}, //
        {1, 35, 1},  //*
        {-1, 36, 7}  //*
    };
    vector<MemoryBlock> freeBlocks = {{-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{2, 0, 12},  //
                                          {2, 12, 3},  //
                                          {2, 15, 20}, //
                                          {-1, 35, 8}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 35, 8}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }
}

TEST_CASE("test_memory_least_appropriate_strategy") {
  SECTION("Create instance of LeastAppropriateStrategy") {
    auto strategy = LeastAppropriateStrategy::create();

    REQUIRE(strategy->type == StrategyType::LEAST_APPROPRIATE);
  }

  SECTION("Process CreateProcessReq request") {
    auto strategy = LeastAppropriateStrategy::create();
    auto request = CreateProcessReq(3, 16384, 4);

    vector<MemoryBlock> blocks = {{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //*
                                  {2, 35, 1},   //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                      {-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{0, 0, 12},   //
                                          {2, 12, 3},   //
                                          {3, 15, 4},   //
                                          {-1, 19, 16}, //
                                          {2, 35, 1},   //
                                          {-1, 36, 7}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 19, 16}, //
                                              {-1, 36, 7}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process TerminateProcessReq request") {
    auto strategy = LeastAppropriateStrategy::create();
    auto request = TerminateProcessReq(2);

    vector<MemoryBlock> blocks = {
        {-1, 0, 7},  //
        {1, 7, 1},   //
        {2, 8, 12},  //*
        {2, 20, 3},  //*
        {2, 23, 20}, //*
    };
    vector<MemoryBlock> freeBlocks = {{-1, 0, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{-1, 0, 7}, //
                                          {1, 7, 1},  //
                                          {-1, 8, 35}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 8, 35}, //
                                              {-1, 0, 7}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process AllocateMemory request") {
    auto strategy = LeastAppropriateStrategy::create();
    auto request = AllocateMemory(1, 8192, 2);

    vector<MemoryBlock> blocks = {{2, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {1, 35, 1},   //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {{-1, 15, 20}, //*
                                      {-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{2, 0, 12},   //
                                          {2, 12, 3},   //
                                          {1, 15, 2},   //
                                          {-1, 17, 18}, //
                                          {1, 35, 1},   //
                                          {-1, 36, 7}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 17, 18}, //
                                              {-1, 36, 7}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process AllocateMemory request (with defragmentation)") {
    auto strategy = LeastAppropriateStrategy::create();
    auto request = AllocateMemory(1, 90112, 22);

    vector<MemoryBlock> blocks = {{2, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {1, 35, 1},   //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {
        {-1, 15, 20}, //*
        {-1, 36, 7}   //*
    };
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{2, 0, 12},  //
                                          {2, 12, 3},  //
                                          {1, 15, 1},  //
                                          {1, 16, 22}, //
                                          {-1, 38, 5}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 38, 5}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process AllocateMemory request (out of memory)") {
    auto strategy = LeastAppropriateStrategy::create();
    auto request = AllocateMemory(1, 122800, 30);

    vector<MemoryBlock> blocks = {{2, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {1, 35, 1},   //
                                  {-1, 36, 7}};
    vector<MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                      {-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{2, 0, 12},   //
                                          {2, 12, 3},   //
                                          {-1, 15, 20}, //
                                          {1, 35, 1},   //
                                          {-1, 36, 7}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 15, 20}, //
                                              {-1, 36, 7}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Process FreeMemory request") {
    auto strategy = LeastAppropriateStrategy::create();
    auto request = FreeMemory(1, 35);

    vector<MemoryBlock> blocks = {
        {2, 0, 12},  //
        {2, 12, 3},  //
        {2, 15, 20}, //
        {1, 35, 1},  //*
        {-1, 36, 7}  //*
    };
    vector<MemoryBlock> freeBlocks = {MemoryBlock{-1, 36, 7}};
    MemoryState state(blocks, freeBlocks);

    vector<MemoryBlock> expectedBlocks = {{2, 0, 12},  //
                                          {2, 12, 3},  //
                                          {2, 15, 20}, //
                                          {-1, 35, 8}};
    vector<MemoryBlock> expectedFreeBlocks = {{-1, 35, 8}};
    MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = strategy->processRequest(request, state);
    REQUIRE(actualState == expectedState);
  }
}

TEST_CASE("test_memory_types_block") {
  SECTION("Create valid instance of MemoryBlock") {
    MemoryBlock block{-1, 0, 10};

    REQUIRE(block.pid() == -1);
    REQUIRE(block.address() == 0);
    REQUIRE(block.size() == 10);
  }

  SECTION("Create invalid instance of MemoryBlock") {
    // Invalid PID
    REQUIRE_THROWS_AS(MemoryBlock(-2, 0, 10), TypeException);
    REQUIRE_THROWS_AS(MemoryBlock(256, 0, 10), TypeException);

    // Invalid address
    REQUIRE_THROWS_AS(MemoryBlock(0, -1, 10), TypeException);
    REQUIRE_THROWS_AS(MemoryBlock(0, 256, 10), TypeException);

    // Invalid size
    REQUIRE_THROWS_AS(MemoryBlock(0, 0, 0), TypeException);
    REQUIRE_THROWS_AS(MemoryBlock(0, 0, 300), TypeException);

    // Memory block is out of bound
    REQUIRE_THROWS_AS(MemoryBlock(0, 200, 100), TypeException);
  }
}
