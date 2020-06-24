#include <vector>

#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include <algo/memory/exceptions.h>
#include <algo/memory/types.h>

namespace mm = MemoryManagement;
using std::vector;

TEST_CASE("MemoryManagement::MemoryBlock") {
  SECTION("Создать экземпляр MemoryBlock (свободный блок)") {
    mm::MemoryBlock block{-1, 0, 10};

    REQUIRE(block.pid() == -1);
    REQUIRE(block.address() == 0);
    REQUIRE(block.size() == 10);
  }

  SECTION("Создать экземпляр MemoryBlock (занятый блок)") {
    mm::MemoryBlock block{255, 0, 10};

    REQUIRE(block.pid() == 255);
    REQUIRE(block.address() == 0);
    REQUIRE(block.size() == 10);
  }

  SECTION("Получить JSON экземпляра MemoryBlock") {
    mm::MemoryBlock block{255, 0, 10};

    auto expected = nlohmann::json{{"pid", 255},   //
                                   {"address", 0}, //
                                   {"size", 10}};
    auto actual = block.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Ограничения на параметры MemoryBlock") {
    // Некорректный PID
    REQUIRE_THROWS_AS(mm::MemoryBlock(-2, 0, 10), mm::TypeException);
    REQUIRE_THROWS_AS(mm::MemoryBlock(256, 0, 10), mm::TypeException);

    // Некорректный адрес начала блока
    REQUIRE_THROWS_AS(mm::MemoryBlock(0, -1, 10), mm::TypeException);
    REQUIRE_THROWS_AS(mm::MemoryBlock(0, 256, 10), mm::TypeException);

    // Некорректный размер
    REQUIRE_THROWS_AS(mm::MemoryBlock(0, 0, 0), mm::TypeException);
    REQUIRE_THROWS_AS(mm::MemoryBlock(0, 0, 257), mm::TypeException);

    // Несоответствие между размером и адресом начала блока
    REQUIRE_THROWS_AS(mm::MemoryBlock(0, 1, 256), mm::TypeException);
  }
}

TEST_CASE("MemoryManagement::MemoryState") {
  SECTION("Создать экземпляр MemoryState") {
    vector<mm::MemoryBlock> blocks = {{0, 0, 12},   //
                                      {2, 12, 3},   //
                                      {-1, 15, 20}, //
                                      {2, 35, 1},   //
                                      {-1, 36, 7}};
    vector<mm::MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                          {-1, 36, 7}};
    mm::MemoryState state(blocks, freeBlocks);

    REQUIRE(state.blocks == blocks);
    REQUIRE(state.freeBlocks == freeBlocks);
  }

  SECTION("Создать экземпляр MemoryState (начальное состояние)") {
    auto actual = mm::MemoryState::initial();

    auto expected = mm::MemoryState{{{-1, 0, 256}}, {{-1, 0, 256}}};

    REQUIRE(actual == expected);
  }

  SECTION("Получить JSON экземпляра MemoryState") {
    vector<mm::MemoryBlock> blocks = {{0, 0, 12},   //
                                      {2, 12, 3},   //
                                      {-1, 15, 20}, //
                                      {2, 35, 1},   //
                                      {-1, 36, 7}};
    vector<mm::MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                          {-1, 36, 7}};
    mm::MemoryState state(blocks, freeBlocks);

    auto expected = R"(
        {
            "blocks": [
                {"pid":  0, "address":  0, "size": 12},
                {"pid":  2, "address": 12, "size":  3},
                {"pid": -1, "address": 15, "size": 20},
                {"pid":  2, "address": 35, "size":  1},
                {"pid": -1, "address": 36, "size":  7}
            ],
            "free_blocks": [
                {"pid": -1, "address": 15, "size": 20},
                {"pid": -1, "address": 36, "size":  7}
            ]
        }
      )"_json;
    auto actual = state.dump();

    REQUIRE(actual == expected);
  }
}
