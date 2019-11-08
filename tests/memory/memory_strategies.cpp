#include <vector>

#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include <algo/memory/requests.h>
#include <algo/memory/strategies.h>
#include <algo/memory/types.h>

namespace mm = MemoryManagement;

TEST_CASE("MemoryManagement::FirstAppropriateStrategy") {
  SECTION("Создать экземпляр FirstAppropriateStrategy") {
    auto strategy = mm::FirstAppropriateStrategy::create();

    REQUIRE(strategy->type == mm::StrategyType::FIRST_APPROPRIATE);
  }

  SECTION("Обработать заявку CreateProcessReq") {
    auto strategy = mm::FirstAppropriateStrategy::create();
    auto request = mm::CreateProcessReq(3, 4 * 4096);

    auto state = mm::MemoryState{
        {{0, 0, 12}, //
         {2, 12, 3}, //
         {-1, 15, 20}, //<- блок, в котором будет выделена память
         {2, 35, 1}, //
         {-1, 36, 7}},
        {{-1, 15, 20}, //
         {-1, 36, 7}}};

    auto expected = mm::MemoryState{{{0, 0, 12}, //
                                     {2, 12, 3}, //
                                     {3, 15, 4}, //
                                     {-1, 19, 16}, //<- новый свободный блок
                                     {2, 35, 1}, //
                                     {-1, 36, 7}},
                                    {{-1, 19, 16}, //
                                     {-1, 36, 7}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку CreateProcessReq (с дефрагментацией)") {
    auto strategy = mm::FirstAppropriateStrategy::create();
    auto request = mm::CreateProcessReq(3, 21 * 4096);

    auto state =
        mm::MemoryState{{
                            {0, 0, 12}, //
                            {2, 12, 3}, //
                            {-1, 15, 20}, //<- блоки, которые будут объединены
                            {2, 35, 1}, //
                            {-1, 36, 7} //<-
                        },
                        {{-1, 15, 20}, //
                         {-1, 36, 7}}};

    auto expected = mm::MemoryState{{
                                        {0, 0, 12},  //
                                        {2, 12, 3},  //
                                        {2, 15, 1},  //
                                        {3, 16, 21}, //
                                        {-1, 37, 6}, //<- новый свободный блок
                                    },
                                    {{-1, 37, 6}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку CreateProcessReq (недостаточно памяти)") {
    auto strategy = mm::FirstAppropriateStrategy::create();
    auto request = mm::CreateProcessReq(3, 28 * 4096);

    auto state = mm::MemoryState{{{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {2, 35, 1},   //
                                  {-1, 36, 7}},
                                 {{-1, 15, 20}, //
                                  {-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку CreateProcessReq (процесс уже создан)") {
    auto strategy = mm::FirstAppropriateStrategy::create();
    auto request = mm::CreateProcessReq(2, 4 * 4096);

    auto state = mm::MemoryState{{{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {2, 35, 1},   //
                                  {-1, 36, 7}},
                                 {{-1, 15, 20}, //
                                  {-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку TerminateProcessReq") {
    auto strategy = mm::FirstAppropriateStrategy::create();
    auto request = mm::TerminateProcessReq(2);

    auto state =
        mm::MemoryState{{{2, 0, 12}, //<-
                         {2, 12, 3}, //<- блоки, которые будут освобождены
                         {2, 15, 20}, //<-
                         {1, 35, 1},  //
                         {-1, 36, 7}},
                        {{-1, 36, 7}}};

    auto expected = mm::MemoryState{{{-1, 0, 35}, //<- новый свободный блок
                                     {1, 35, 1}, //
                                     {-1, 36, 7}},
                                    {{-1, 0, 35}, //
                                     {-1, 36, 7}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку TerminateProcessReq (процесс не существует)") {
    auto strategy = mm::FirstAppropriateStrategy::create();
    auto request = mm::TerminateProcessReq(3);

    auto state = mm::MemoryState{{{2, 0, 12},  //
                                  {2, 12, 3},  //
                                  {2, 15, 20}, //
                                  {1, 35, 1},  //
                                  {-1, 36, 7}},
                                 {{-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку AllocateMemory") {
    auto strategy = mm::FirstAppropriateStrategy::create();
    auto request = mm::AllocateMemory(2, 4 * 4096);

    auto state = mm::MemoryState{
        {{0, 0, 12}, //
         {2, 12, 3}, //
         {-1, 15, 20}, //<- блок, в котором будет выделена память
         {2, 35, 1}, //
         {-1, 36, 7}},
        {{-1, 15, 20}, //
         {-1, 36, 7}}};

    auto expected = mm::MemoryState{{{0, 0, 12}, //
                                     {2, 12, 3}, //
                                     {2, 15, 4}, //
                                     {-1, 19, 16}, //<- новый свободный блок
                                     {2, 35, 1}, //
                                     {-1, 36, 7}},
                                    {{-1, 19, 16}, //
                                     {-1, 36, 7}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку AllocateMemory (с дефрагментацией)") {
    auto strategy = mm::FirstAppropriateStrategy::create();
    auto request = mm::AllocateMemory(2, 21 * 4096);

    auto state =
        mm::MemoryState{{
                            {0, 0, 12}, //
                            {2, 12, 3}, //
                            {-1, 15, 20}, //<- блоки, которые будут объединены
                            {2, 35, 1}, //
                            {-1, 36, 7} //<-
                        },
                        {{-1, 15, 20}, //
                         {-1, 36, 7}}};

    auto expected = mm::MemoryState{{
                                        {0, 0, 12},  //
                                        {2, 12, 3},  //
                                        {2, 15, 1},  //
                                        {2, 16, 21}, //
                                        {-1, 37, 6}, //<- новый свободный блок
                                    },
                                    {{-1, 37, 6}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку AllocateMemory (недостаточно памяти)") {
    auto strategy = mm::FirstAppropriateStrategy::create();
    auto request = mm::AllocateMemory(2, 28 * 4096);

    auto state = mm::MemoryState{{{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {2, 35, 1},   //
                                  {-1, 36, 7}},
                                 {{-1, 15, 20}, //
                                  {-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку AllocateMemory (процесс не существует)") {
    auto strategy = mm::FirstAppropriateStrategy::create();
    auto request = mm::AllocateMemory(3, 4 * 4096);

    auto state = mm::MemoryState{{{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {2, 35, 1},   //
                                  {-1, 36, 7}},
                                 {{-1, 15, 20}, //
                                  {-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку FreeMemory") {
    auto strategy = mm::FirstAppropriateStrategy::create();
    auto request = mm::FreeMemory(2, 15);

    auto state =
        mm::MemoryState{{
                            {2, 0, 12}, //
                            {2, 12, 3}, //
                            {2, 15, 20}, //<- блок, который будет освобожден
                            {1, 35, 1}, //
                            {-1, 36, 7} //
                        },
                        {{-1, 36, 7}}};

    auto expected = mm::MemoryState{{
                                        {2, 0, 12}, //
                                        {2, 12, 3}, //
                                        {-1, 15, 20}, //<- новый свободный блок
                                        {1, 35, 1}, //
                                        {-1, 36, 7} //
                                    },
                                    {{-1, 15, 20}, //
                                     {-1, 36, 7}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку FreeMemory (несуществующий блок)") {
    auto strategy = mm::FirstAppropriateStrategy::create();
    auto request = mm::FreeMemory(1, 34);

    auto state = mm::MemoryState{{
                                     {2, 0, 12},  //
                                     {2, 12, 3},  //
                                     {2, 15, 20}, //
                                     {1, 35, 1},  //
                                     {-1, 36, 7}  //
                                 },
                                 {{-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку FreeMemory (блок принадлежит другому процессу)") {
    auto strategy = mm::FirstAppropriateStrategy::create();
    auto request = mm::FreeMemory(2, 35);

    auto state = mm::MemoryState{{
                                     {2, 0, 12},  //
                                     {2, 12, 3},  //
                                     {2, 15, 20}, //
                                     {1, 35, 1},  //
                                     {-1, 36, 7}  //
                                 },
                                 {{-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }
}

TEST_CASE("MemoryManagement::MostAppropriateStrategy") {
  SECTION("Создать экземпляр MostAppropriateStrategy") {
    auto strategy = mm::MostAppropriateStrategy::create();

    REQUIRE(strategy->type == mm::StrategyType::MOST_APPROPRIATE);
  }

  SECTION("Обработать заявку CreateProcessReq") {
    auto strategy = mm::MostAppropriateStrategy::create();
    auto request = mm::CreateProcessReq(3, 4 * 4096);

    auto state = mm::MemoryState{
        {
            {0, 0, 12},   //
            {2, 12, 3},   //
            {-1, 15, 20}, //
            {2, 35, 1},   //
            {-1, 36, 7} //<- блок, в котором будет выделена память
        },
        {{-1, 36, 7}, //
         {-1, 15, 20}}};

    auto expected = mm::MemoryState{{
                                        {0, 0, 12},   //
                                        {2, 12, 3},   //
                                        {-1, 15, 20}, //
                                        {2, 35, 1},   //
                                        {3, 36, 4},   //
                                        {-1, 40, 3} //<- новый свободный блок
                                    },
                                    {{-1, 40, 3}, //
                                     {-1, 15, 20}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку CreateProcessReq (с дефрагментацией)") {
    auto strategy = mm::MostAppropriateStrategy::create();
    auto request = mm::CreateProcessReq(3, 21 * 4096);

    auto state =
        mm::MemoryState{{
                            {0, 0, 12}, //
                            {2, 12, 3}, //
                            {-1, 15, 20}, //<- блоки, которые будут объединены
                            {2, 35, 1}, //
                            {-1, 36, 7} //<-
                        },
                        {{-1, 36, 7}, //
                         {-1, 15, 20}}};

    auto expected = mm::MemoryState{{
                                        {0, 0, 12},  //
                                        {2, 12, 3},  //
                                        {2, 15, 1},  //
                                        {3, 16, 21}, //
                                        {-1, 37, 6}, //<- новый свободный блок
                                    },
                                    {{-1, 37, 6}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку CreateProcessReq (недостаточно памяти)") {
    auto strategy = mm::MostAppropriateStrategy::create();
    auto request = mm::CreateProcessReq(3, 28 * 4096);

    auto state = mm::MemoryState{{{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {2, 35, 1},   //
                                  {-1, 36, 7}},
                                 {{-1, 36, 7}, //
                                  {-1, 15, 20}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку CreateProcessReq (процесс уже создан)") {
    auto strategy = mm::MostAppropriateStrategy::create();
    auto request = mm::CreateProcessReq(2, 4 * 4096);

    auto state = mm::MemoryState{{{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {2, 35, 1},   //
                                  {-1, 36, 7}},
                                 {{-1, 36, 7}, //
                                  {-1, 15, 20}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку TerminateProcessReq") {
    auto strategy = mm::MostAppropriateStrategy::create();
    auto request = mm::TerminateProcessReq(2);

    auto state =
        mm::MemoryState{{{2, 0, 12}, //<-
                         {2, 12, 3}, //<- блоки, которые будут освобождены
                         {2, 15, 20}, //<-
                         {1, 35, 1},  //
                         {-1, 36, 7}},
                        {{-1, 36, 7}}};

    auto expected = mm::MemoryState{{{-1, 0, 35}, //<- новый свободный блок
                                     {1, 35, 1}, //
                                     {-1, 36, 7}},
                                    {{-1, 36, 7}, //
                                     {-1, 0, 35}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку TerminateProcessReq (процесс не существует)") {
    auto strategy = mm::MostAppropriateStrategy::create();
    auto request = mm::TerminateProcessReq(3);

    auto state = mm::MemoryState{{{2, 0, 12},  //
                                  {2, 12, 3},  //
                                  {2, 15, 20}, //
                                  {1, 35, 1},  //
                                  {-1, 36, 7}},
                                 {{-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку AllocateMemory") {
    auto strategy = mm::MostAppropriateStrategy::create();
    auto request = mm::AllocateMemory(2, 4 * 4096);

    auto state = mm::MemoryState{
        {
            {0, 0, 12},   //
            {2, 12, 3},   //
            {-1, 15, 20}, //
            {2, 35, 1},   //
            {-1, 36, 7} //<- блок, в котором будет выделена память
        },
        {{-1, 36, 7}, //
         {-1, 15, 20}}};

    auto expected = mm::MemoryState{{
                                        {0, 0, 12},   //
                                        {2, 12, 3},   //
                                        {-1, 15, 20}, //
                                        {2, 35, 1},   //
                                        {2, 36, 4},   //
                                        {-1, 40, 3} //<- новый свободный блок
                                    },
                                    {{-1, 40, 3}, //
                                     {-1, 15, 20}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку AllocateMemory (с дефрагментацией)") {
    auto strategy = mm::MostAppropriateStrategy::create();
    auto request = mm::AllocateMemory(2, 21 * 4096);

    auto state =
        mm::MemoryState{{
                            {0, 0, 12}, //
                            {2, 12, 3}, //
                            {-1, 15, 20}, //<- блоки, которые будут объединены
                            {2, 35, 1}, //
                            {-1, 36, 7} //<-
                        },
                        {{-1, 36, 7}, //
                         {-1, 15, 20}}};

    auto expected = mm::MemoryState{{
                                        {0, 0, 12},  //
                                        {2, 12, 3},  //
                                        {2, 15, 1},  //
                                        {2, 16, 21}, //
                                        {-1, 37, 6}, //<- новый свободный блок
                                    },
                                    {{-1, 37, 6}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку AllocateMemory (недостаточно памяти)") {
    auto strategy = mm::MostAppropriateStrategy::create();
    auto request = mm::AllocateMemory(2, 28 * 4096);

    auto state = mm::MemoryState{{{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {2, 35, 1},   //
                                  {-1, 36, 7}},
                                 {{-1, 36, 7}, //
                                  {-1, 15, 20}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку AllocateMemory (процесс не существует)") {
    auto strategy = mm::MostAppropriateStrategy::create();
    auto request = mm::AllocateMemory(3, 4 * 4096);

    auto state = mm::MemoryState{{{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {2, 35, 1},   //
                                  {-1, 36, 7}},
                                 {{-1, 36, 7}, //
                                  {-1, 15, 20}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку FreeMemory") {
    auto strategy = mm::MostAppropriateStrategy::create();
    auto request = mm::FreeMemory(2, 15);

    auto state =
        mm::MemoryState{{
                            {2, 0, 12}, //
                            {2, 12, 3}, //
                            {2, 15, 20}, //<- блок, который будет освобожден
                            {1, 35, 1}, //
                            {-1, 36, 7} //
                        },
                        {{-1, 36, 7}}};

    auto expected = mm::MemoryState{{
                                        {2, 0, 12}, //
                                        {2, 12, 3}, //
                                        {-1, 15, 20}, //<- новый свободный блок
                                        {1, 35, 1}, //
                                        {-1, 36, 7} //
                                    },
                                    {{-1, 36, 7}, //
                                     {-1, 15, 20}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку FreeMemory (несуществующий блок)") {
    auto strategy = mm::MostAppropriateStrategy::create();
    auto request = mm::FreeMemory(1, 34);

    auto state = mm::MemoryState{{
                                     {2, 0, 12},  //
                                     {2, 12, 3},  //
                                     {2, 15, 20}, //
                                     {1, 35, 1},  //
                                     {-1, 36, 7}  //
                                 },
                                 {{-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку FreeMemory (блок принадлежит другому процессу)") {
    auto strategy = mm::MostAppropriateStrategy::create();
    auto request = mm::FreeMemory(2, 35);

    auto state = mm::MemoryState{{
                                     {2, 0, 12},  //
                                     {2, 12, 3},  //
                                     {2, 15, 20}, //
                                     {1, 35, 1},  //
                                     {-1, 36, 7}  //
                                 },
                                 {{-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }
}

TEST_CASE("MemoryManagement::LeastAppropriateStrategy") {
  SECTION("Создать экземпляр LeastAppropriateStrategy") {
    auto strategy = mm::LeastAppropriateStrategy::create();

    REQUIRE(strategy->type == mm::StrategyType::LEAST_APPROPRIATE);
  }

  SECTION("Обработать заявку CreateProcessReq") {
    auto strategy = mm::LeastAppropriateStrategy::create();
    auto request = mm::CreateProcessReq(3, 4 * 4096);

    auto state = mm::MemoryState{
        {
            {0, 0, 12}, //
            {2, 12, 3}, //
            {-1, 15, 20}, //<- блок, в котором будет выделена память
            {2, 35, 1}, //
            {-1, 36, 7} //
        },
        {{-1, 15, 20}, //
         {-1, 36, 7}}};

    auto expected = mm::MemoryState{{
                                        {0, 0, 12}, //
                                        {2, 12, 3}, //
                                        {3, 15, 4}, //
                                        {-1, 19, 16}, //<- новый свободный блок
                                        {2, 35, 1}, //
                                        {-1, 36, 7} //
                                    },
                                    {{-1, 19, 16}, //
                                     {-1, 36, 7}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку CreateProcessReq (с дефрагментацией)") {
    auto strategy = mm::LeastAppropriateStrategy::create();
    auto request = mm::CreateProcessReq(3, 21 * 4096);

    auto state =
        mm::MemoryState{{
                            {0, 0, 12}, //
                            {2, 12, 3}, //
                            {-1, 15, 20}, //<- блоки, которые будут объединены
                            {2, 35, 1}, //
                            {-1, 36, 7} //<-
                        },
                        {{-1, 15, 20}, //
                         {-1, 36, 7}}};

    auto expected = mm::MemoryState{{
                                        {0, 0, 12},  //
                                        {2, 12, 3},  //
                                        {2, 15, 1},  //
                                        {3, 16, 21}, //
                                        {-1, 37, 6}, //<- новый свободный блок
                                    },
                                    {{-1, 37, 6}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку CreateProcessReq (недостаточно памяти)") {
    auto strategy = mm::LeastAppropriateStrategy::create();
    auto request = mm::CreateProcessReq(3, 28 * 4096);

    auto state = mm::MemoryState{{{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {2, 35, 1},   //
                                  {-1, 36, 7}},
                                 {{-1, 15, 20}, //
                                  {-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку CreateProcessReq (процесс уже создан)") {
    auto strategy = mm::LeastAppropriateStrategy::create();
    auto request = mm::CreateProcessReq(2, 4 * 4096);

    auto state = mm::MemoryState{{{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {2, 35, 1},   //
                                  {-1, 36, 7}},
                                 {{-1, 15, 20}, //
                                  {-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку TerminateProcessReq") {
    auto strategy = mm::LeastAppropriateStrategy::create();
    auto request = mm::TerminateProcessReq(2);

    auto state =
        mm::MemoryState{{{2, 0, 12}, //<-
                         {2, 12, 3}, //<- блоки, которые будут освобождены
                         {2, 15, 20}, //<-
                         {1, 35, 1},  //
                         {-1, 36, 7}},
                        {{-1, 36, 7}}};

    auto expected = mm::MemoryState{{{-1, 0, 35}, //<- новый свободный блок
                                     {1, 35, 1}, //
                                     {-1, 36, 7}},
                                    {{-1, 0, 35}, //
                                     {-1, 36, 7}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку TerminateProcessReq (процесс не существует)") {
    auto strategy = mm::LeastAppropriateStrategy::create();
    auto request = mm::TerminateProcessReq(3);

    auto state = mm::MemoryState{{{2, 0, 12},  //
                                  {2, 12, 3},  //
                                  {2, 15, 20}, //
                                  {1, 35, 1},  //
                                  {-1, 36, 7}},
                                 {{-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку AllocateMemory") {
    auto strategy = mm::LeastAppropriateStrategy::create();
    auto request = mm::AllocateMemory(2, 4 * 4096);

    auto state = mm::MemoryState{
        {
            {0, 0, 12}, //
            {2, 12, 3}, //
            {-1, 15, 20}, //<- блок, в котором будет выделена память
            {2, 35, 1}, //
            {-1, 36, 7} //
        },
        {{-1, 15, 20}, //
         {-1, 36, 7}}};

    auto expected = mm::MemoryState{{
                                        {0, 0, 12}, //
                                        {2, 12, 3}, //
                                        {2, 15, 4}, //
                                        {-1, 19, 16}, //<- новый свободный блок
                                        {2, 35, 1},  //
                                        {-1, 36, 7}, //
                                    },
                                    {{-1, 19, 16}, //
                                     {-1, 36, 7}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку AllocateMemory (с дефрагментацией)") {
    auto strategy = mm::LeastAppropriateStrategy::create();
    auto request = mm::AllocateMemory(2, 21 * 4096);

    auto state =
        mm::MemoryState{{
                            {0, 0, 12}, //
                            {2, 12, 3}, //
                            {-1, 15, 20}, //<- блоки, которые будут объединены
                            {2, 35, 1}, //
                            {-1, 36, 7} //<-
                        },
                        {{-1, 15, 20}, //
                         {-1, 36, 7}}};

    auto expected = mm::MemoryState{{
                                        {0, 0, 12},  //
                                        {2, 12, 3},  //
                                        {2, 15, 1},  //
                                        {2, 16, 21}, //
                                        {-1, 37, 6}, //<- новый свободный блок
                                    },
                                    {{-1, 37, 6}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку AllocateMemory (недостаточно памяти)") {
    auto strategy = mm::LeastAppropriateStrategy::create();
    auto request = mm::AllocateMemory(2, 28 * 4096);

    auto state = mm::MemoryState{{{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {2, 35, 1},   //
                                  {-1, 36, 7}},
                                 {{-1, 15, 20}, //
                                  {-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку AllocateMemory (процесс не существует)") {
    auto strategy = mm::LeastAppropriateStrategy::create();
    auto request = mm::AllocateMemory(3, 4 * 4096);

    auto state = mm::MemoryState{{{0, 0, 12},   //
                                  {2, 12, 3},   //
                                  {-1, 15, 20}, //
                                  {2, 35, 1},   //
                                  {-1, 36, 7}},
                                 {{-1, 15, 20}, //
                                  {-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку FreeMemory") {
    auto strategy = mm::LeastAppropriateStrategy::create();
    auto request = mm::FreeMemory(2, 15);

    auto state =
        mm::MemoryState{{
                            {2, 0, 12}, //
                            {2, 12, 3}, //
                            {2, 15, 20}, //<- блок, который будет освобожден
                            {1, 35, 1}, //
                            {-1, 36, 7} //
                        },
                        {{-1, 36, 7}}};

    auto expected = mm::MemoryState{{
                                        {2, 0, 12}, //
                                        {2, 12, 3}, //
                                        {-1, 15, 20}, //<- новый свободный блок
                                        {1, 35, 1}, //
                                        {-1, 36, 7} //
                                    },
                                    {{-1, 15, 20}, //
                                     {-1, 36, 7}}};
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку FreeMemory (несуществующий блок)") {
    auto strategy = mm::LeastAppropriateStrategy::create();
    auto request = mm::FreeMemory(1, 34);

    auto state = mm::MemoryState{{
                                     {2, 0, 12},  //
                                     {2, 12, 3},  //
                                     {2, 15, 20}, //
                                     {1, 35, 1},  //
                                     {-1, 36, 7}  //
                                 },
                                 {{-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }

  SECTION("Обработать заявку FreeMemory (блок принадлежит другому процессу)") {
    auto strategy = mm::LeastAppropriateStrategy::create();
    auto request = mm::FreeMemory(2, 35);

    auto state = mm::MemoryState{{
                                     {2, 0, 12},  //
                                     {2, 12, 3},  //
                                     {2, 15, 20}, //
                                     {1, 35, 1},  //
                                     {-1, 36, 7}  //
                                 },
                                 {{-1, 36, 7}}};

    auto expected = state;
    auto actual = strategy->processRequest(request, state);
    REQUIRE(actual == expected);
  }
}
