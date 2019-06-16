#include <algorithm>
#include <array>
#include <deque>
#include <vector>

#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include <algo/processes/exceptions.h>
#include <algo/processes/types.h>

namespace pm = ProcessesManagement;
using std::vector;

TEST_CASE("ProcessesManagement::Process") {
  SECTION("Создать экземпляр Process") {
    pm::Process process;

    REQUIRE(process.pid() == 0);
    REQUIRE(process.ppid() == -1);
    REQUIRE(process.priority() == 0);
    REQUIRE(process.basePriority() == 0);
    REQUIRE(process.timer() == 0);
    REQUIRE(process.workTime() == 0);
    REQUIRE(process.state() == pm::ProcState::ACTIVE);
  }

  SECTION("Получить JSON экземпляра Process") {
    auto process = pm::Process{};

    auto expected = nlohmann::json{{"pid", 0},          //
                                   {"ppid", -1},        //
                                   {"priority", 0},     //
                                   {"basePriority", 0}, //
                                   {"timer", 0},        //
                                   {"workTime", 0},     //
                                   {"state", "ACTIVE"}};
    auto actual = process.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Ограничения на параметры Process") {
    // Некорректный PID
    REQUIRE_THROWS_AS(pm::Process{}.pid(-1), pm::TypeException);
    REQUIRE_THROWS_AS(pm::Process{}.pid(256), pm::TypeException);

    // Некорректный PPID
    REQUIRE_THROWS_AS(pm::Process{}.ppid(-2), pm::TypeException);
    REQUIRE_THROWS_AS(pm::Process{}.ppid(256), pm::TypeException);

    // Некорректный приоритет
    REQUIRE_THROWS_AS(pm::Process{}.priority(16), pm::TypeException);

    // Некорректный базовый приоритет
    REQUIRE_THROWS_AS(pm::Process{}.basePriority(16), pm::TypeException);

    // Текущий приоритет меньше базового
    REQUIRE_THROWS_AS(pm::Process{}.priority(0).basePriority(1),
                      pm::TypeException);
    REQUIRE_THROWS_AS(pm::Process{}.priority(2).basePriority(1).priority(0),
                      pm::TypeException);

    // Некорректное время работы
    REQUIRE_THROWS_AS(pm::Process{}.timer(-1),
                      ProcessesManagement::TypeException);

    // Некорректное заявленное время работы
    REQUIRE_THROWS_AS(pm::Process{}.workTime(-1),
                      ProcessesManagement::TypeException);
  }
}

TEST_CASE("ProcessesManagement::ProcessesState") {
  SECTION("Создать экземпляр ProcessesState") {
    pm::ProcessesState state{// processes
                             {pm::Process{}.pid(1)},
                             // queues
                             {{0, {1}}}};

    REQUIRE(state.processes.size() == 1);
    REQUIRE(state.processes.at(0) == pm::Process{}.pid(1));

    bool empty = std::all_of(state.queues.begin() + 1, state.queues.end(),
                             [](const auto &queue) { return queue.empty(); });
    REQUIRE(empty);
    REQUIRE(state.queues.at(0).size() == 1);
    REQUIRE(state.queues.at(0).at(0) == 1);
  }

  SECTION("Создать экземпляр ProcessesState (начальное состояние)") {
    auto state = pm::ProcessesState::initial();

    REQUIRE(state.processes.size() == 0);
    bool empty = std::all_of(state.queues.begin(), state.queues.end(),
                             [](const auto &queue) { return queue.empty(); });
    REQUIRE(empty);
  }

  SECTION("Получить JSON экземпляра ProcessesState") {
    pm::ProcessesState state{// processes
                             {pm::Process{}.pid(1)},
                             // queues
                             {{0, {1}}}};

    auto expected = R"(
        {
            "processes": [
                {
                    "pid":  1,
                    "ppid": -1,
                    "priority": 0,
                    "basePriority": 0,
                    "timer": 0,
                    "workTime": 0,
                    "state": "ACTIVE"
                }
            ],
            "queues": [
                [1], [], [], [],
                [], [], [], [],
                [], [], [], [],
                [], [], [], []
            ]
        }
      )"_json;
    auto actual = state.dump();

    REQUIRE(actual == expected);
  }
}
