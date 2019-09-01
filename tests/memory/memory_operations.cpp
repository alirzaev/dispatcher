#include <stdexcept>
#include <vector>

#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include <algo/memory/exceptions.h>
#include <algo/memory/operations.h>

namespace mm = MemoryManagement;
using std::vector;

TEST_CASE("MemoryManagement::allocateMemory") {
  SECTION("Выделение памяти (выделяется часть блока)") {
    vector<mm::MemoryBlock> blocks = {
        {0, 0, 12}, //
        {2, 12, 3}, //
        {-1, 15, 20}, //<- блок, где будет выделена память
        {2, 35, 1}, //
        {-1, 36, 7}};
    vector<mm::MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                          {-1, 36, 7}};
    mm::MemoryState state(blocks, freeBlocks);

    vector<mm::MemoryBlock> expectedBlocks = {
        {0, 0, 12},   //
        {2, 12, 3},   //
        {3, 15, 4},   //<- выделенная процессу память
        {-1, 19, 16}, //<- новый свободный блок
        {2, 35, 1},   //
        {-1, 36, 7}};
    vector<mm::MemoryBlock> expectedFreeBlocks = {{-1, 36, 7}, //
                                                  {-1, 19, 16}};
    mm::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = allocateMemory(state, 2, 3, 4);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Выделение памяти (выделяется целый блок)") {
    vector<mm::MemoryBlock> blocks = {
        {0, 0, 12}, //
        {2, 12, 3}, //
        {-1, 15, 20}, //<- блок, где будет выделена память
        {2, 35, 1}, //
        {-1, 36, 7}};
    vector<mm::MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                          {-1, 36, 7}};
    mm::MemoryState state(blocks, freeBlocks);

    vector<mm::MemoryBlock> expectedBlocks = {
        {0, 0, 12},  //
        {2, 12, 3},  //
        {3, 15, 20}, //<- выделенная процессу память
        {2, 35, 1},  //
        {-1, 36, 7}};
    vector<mm::MemoryBlock> expectedFreeBlocks = {{-1, 36, 7}};
    mm::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = allocateMemory(state, 2, 3, 20);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Выделение памяти (попытка выделить занятый блок)") {
    vector<mm::MemoryBlock> blocks = {
        {0, 0, 12}, //
        {2, 12, 3}, //<- блок, где будет выделена память
        {-1, 15, 20}, //
        {2, 35, 1},   //
        {-1, 36, 7}};
    vector<mm::MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                          {-1, 36, 7}};
    mm::MemoryState state(blocks, freeBlocks);

    REQUIRE_THROWS_AS(allocateMemory(state, 1, 3, 3), mm::OperationException);
  }

  SECTION("Выделение памяти (блок слишком мал)") {
    vector<mm::MemoryBlock> blocks = {
        {0, 0, 12}, //
        {2, 12, 3}, //<- блок, где будет выделена память
        {-1, 15, 20}, //
        {2, 35, 1},   //
        {-1, 36, 7}};
    vector<mm::MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                          {-1, 36, 7}};
    mm::MemoryState state(blocks, freeBlocks);

    REQUIRE_THROWS_AS(allocateMemory(state, 2, 3, 21), mm::OperationException);
  }

  SECTION("Выделение памяти (некорректный индекс блока)") {
    vector<mm::MemoryBlock> blocks = {{0, 0, 12},   //
                                      {2, 12, 3},   //
                                      {-1, 15, 20}, //
                                      {2, 35, 1},   //
                                      {-1, 36, 7}};
    vector<mm::MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                          {-1, 36, 7}};
    mm::MemoryState state(blocks, freeBlocks);

    REQUIRE_THROWS_AS(allocateMemory(state, 5, 3, 21), std::out_of_range);
  }
}

TEST_CASE("MemoryManagement::freeMemory") {
  SECTION("Освобождение памяти") {
    vector<mm::MemoryBlock> blocks = {
        {0, 0, 12},   //
        {2, 12, 3},   //
        {-1, 15, 20}, //
        {2, 35, 1}, //<- блок, который будет освобожден
        {-1, 36, 7}};
    vector<mm::MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                          {-1, 36, 7}};
    mm::MemoryState state(blocks, freeBlocks);

    vector<mm::MemoryBlock> expectedBlocks = {
        {0, 0, 12},   //
        {2, 12, 3},   //
        {-1, 15, 20}, //
        {-1, 35, 1},  //<- новый свободный блок
        {-1, 36, 7}};
    vector<mm::MemoryBlock> expectedFreeBlocks = {
        {-1, 15, 20}, //
        {-1, 36, 7},  //
        {-1, 35, 1}}; //<- новый блок добавлен в конец списка
    mm::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = freeMemory(state, 2, 3);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Освобождение памяти (несовпадение PID'ов)") {
    vector<mm::MemoryBlock> blocks = {
        {0, 0, 12},   //
        {2, 12, 3},   //
        {-1, 15, 20}, //
        {2, 35, 1}, //<- блок, который будет освобожден
        {-1, 36, 7}};
    vector<mm::MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                          {-1, 36, 7}};
    mm::MemoryState state(blocks, freeBlocks);

    REQUIRE_THROWS_AS(freeMemory(state, 3, 3), mm::OperationException);
  }

  SECTION("Освобождение памяти (некорректный индекс блока)") {
    vector<mm::MemoryBlock> blocks = {
        {0, 0, 12},   //
        {2, 12, 3},   //
        {-1, 15, 20}, //
        {2, 35, 1}, //<- блок, который будет освобожден
        {-1, 36, 7}};
    vector<mm::MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                          {-1, 36, 7}};
    mm::MemoryState state(blocks, freeBlocks);

    REQUIRE_THROWS_AS(freeMemory(state, 2, 5), std::out_of_range);
  }
}

TEST_CASE("MemoryManagement::defragmentMemory") {
  SECTION("Дефрагментация памяти (несколько свободных блоков)") {
    vector<mm::MemoryBlock> blocks = {
        {0, 0, 12},   //
        {2, 12, 3},   //
        {-1, 15, 20}, //<- свободный блок
        {2, 35, 1},   //
        {-1, 36, 7}   //<- свободный блок
    };
    vector<mm::MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                          {-1, 36, 7}};
    mm::MemoryState state(blocks, freeBlocks);

    vector<mm::MemoryBlock> expectedBlocks = {
        {0, 0, 12},  //
        {2, 12, 3},  //
        {2, 15, 1},  //
        {-1, 16, 27} //<- новый свободный блок
    };
    vector<mm::MemoryBlock> expectedFreeBlocks = {{-1, 16, 27}};
    mm::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = defragmentMemory(state);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Дефрагментация памяти (один свободный блок)") {
    vector<mm::MemoryBlock> blocks = {
        {0, 0, 12},   //
        {2, 12, 3},   //
        {-1, 15, 20}, //<- свободный блок
        {2, 35, 1}    //
    };
    vector<mm::MemoryBlock> freeBlocks = {{-1, 15, 20}};
    mm::MemoryState state(blocks, freeBlocks);

    vector<mm::MemoryBlock> expectedBlocks = {
        {0, 0, 12},  //
        {2, 12, 3},  //
        {2, 15, 1},  //
        {-1, 16, 20} //<- новый свободный блок
    };
    vector<mm::MemoryBlock> expectedFreeBlocks = {{-1, 16, 20}};
    mm::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = defragmentMemory(state);
    REQUIRE(actualState == expectedState);
  }
}

TEST_CASE("MemoryManagement::compressMemory") {
  SECTION("Сжатие памяти") {
    vector<mm::MemoryBlock> blocks = {
        {0, 0, 12},   //
        {2, 12, 3},   //
        {-1, 15, 20}, //<
        {-1, 35, 1}, //<- свободные блоки, которые будут сжаты
        {-1, 36, 7}, //<
        {4, 43, 12}, //
        {-1, 55, 2}, //
        {-1, 57, 6}};
    vector<mm::MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                          {-1, 35, 1},  //
                                          {-1, 36, 7},  //
                                          {-1, 55, 2},  //
                                          {-1, 57, 6}};
    mm::MemoryState state(blocks, freeBlocks);

    vector<mm::MemoryBlock> expectedBlocks = {
        {0, 0, 12},   //
        {2, 12, 3},   //
        {-1, 15, 28}, //<- новый свободный блок
        {4, 43, 12},  //
        {-1, 55, 2},  //
        {-1, 57, 6}};
    vector<mm::MemoryBlock> expectedFreeBlocks = {{-1, 55, 2}, //
                                                  {-1, 57, 6}, //
                                                  {-1, 15, 28}};
    mm::MemoryState expectedState(expectedBlocks, expectedFreeBlocks);

    auto actualState = compressMemory(state, 2);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Сжатие памяти (попытка сжать один блок)") {
    vector<mm::MemoryBlock> blocks = {
        {0, 0, 12},   //
        {2, 12, 3},   //
        {-1, 15, 20}, //
        {-1, 35, 1},  //
        {-1, 36, 7},  //
        {4, 43, 12},  //
        {-1, 55, 2} //<- свободный блок, который надо сжать
    };
    vector<mm::MemoryBlock> freeBlocks = {{-1, 15, 20}, //
                                          {-1, 35, 1},  //
                                          {-1, 36, 7},  //
                                          {-1, 55, 2}};
    mm::MemoryState state(blocks, freeBlocks);

    REQUIRE_THROWS_AS(compressMemory(state, 6), mm::OperationException);
  }
}
