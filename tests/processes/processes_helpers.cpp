#include <array>
#include <deque>

#include <catch2/catch.hpp>

#include <algo/processes/helpers.h>
#include <algo/processes/types.h>

namespace pm = ProcessesManagement;
using Queues = std::array<std::deque<int32_t>, 16>;

TEST_CASE("ProcessesManagement::getIndexByPid") {
  SECTION("Поиск процесса по PID [getIndexByPid(state, index)]") {
    pm::ProcessesState state{                         // processes
                             {pm::Process{}.pid(1),   //
                              pm::Process{}.pid(5),   //
                              pm::Process{}.pid(12)}, //
                                                      // queues
                             Queues{}};

    auto index = pm::getIndexByPid(state, 5);

    REQUIRE(index.has_value());
    REQUIRE(*index == 1);
  }

  SECTION("Поиск процесса по PID [getIndexByPid(processes, index)]") {
    pm::ProcessesState state{                         // processes
                             {pm::Process{}.pid(1),   //
                              pm::Process{}.pid(5),   //
                              pm::Process{}.pid(12)}, //
                                                      // queues
                             Queues{}};

    auto index = pm::getIndexByPid(state.processes, 5);

    REQUIRE(index.has_value());
    REQUIRE(*index == 1);
  }

  SECTION("Поиск процесса по PID (не найден) [getIndexByPid(state, "
          "index)]") {
    pm::ProcessesState state{                         // processes
                             {pm::Process{}.pid(1),   //
                              pm::Process{}.pid(5),   //
                              pm::Process{}.pid(12)}, //
                                                      // queues
                             Queues{}};

    auto index = pm::getIndexByPid(state, 2);

    REQUIRE(!index.has_value());
  }

  SECTION("Поиск процесса по PID (не найден) [getIndexByPid(processes, "
          "index)]") {
    pm::ProcessesState state{                         // processes
                             {pm::Process{}.pid(1),   //
                              pm::Process{}.pid(5),   //
                              pm::Process{}.pid(12)}, //
                                                      // queues
                             Queues{}};

    auto index = pm::getIndexByPid(state.processes, 2);

    REQUIRE(!index.has_value());
  }
}

TEST_CASE("ProcessesManagement::getIndexByState") {
  SECTION("Поиск процесса по procState [getIndexByState(state, procState)]") {
    pm::ProcessesState state{
        // processes
        {pm::Process{}.pid(1),                                 //
         pm::Process{}.pid(5).state(pm::ProcState::EXECUTING), //
         pm::Process{}.pid(12)},                               //
                                                               // queues
        Queues{}};

    auto index = pm::getIndexByState(state, pm::ProcState::EXECUTING);

    REQUIRE(index.has_value());
    REQUIRE(*index == 1);
  }

  SECTION(
      "Поиск процесса по procState (первый по списку) [getIndexByState(state, "
      "procState)]") {
    pm::ProcessesState state{
        // processes
        {pm::Process{}.pid(1).state(pm::ProcState::EXECUTING), //
         pm::Process{}.pid(5),                                 //
         pm::Process{}.pid(12)},                               //
                                                               // queues
        Queues{}};

    auto index = pm::getIndexByState(state, pm::ProcState::ACTIVE);

    REQUIRE(index.has_value());
    REQUIRE(*index == 1);
  }

  SECTION("Поиск процесса по state [getIndexByState(processes, state)]") {
    pm::ProcessesState state{
        // processes
        {pm::Process{}.pid(1),                                 //
         pm::Process{}.pid(5).state(pm::ProcState::EXECUTING), //
         pm::Process{}.pid(12)},                               //
                                                               // queues
        Queues{}};

    auto index = pm::getIndexByState(state.processes, pm::ProcState::EXECUTING);

    REQUIRE(index.has_value());
    REQUIRE(*index == 1);
  }

  SECTION("Поиск процесса по state (первый по списку) "
          "[getIndexByState(processes, state)]") {
    pm::ProcessesState state{
        // processes
        {pm::Process{}.pid(1).state(pm::ProcState::EXECUTING), //
         pm::Process{}.pid(5),                                 //
         pm::Process{}.pid(12)},                               //
                                                               // queues
        Queues{}};

    auto index = pm::getIndexByState(state.processes, pm::ProcState::ACTIVE);

    REQUIRE(index.has_value());
    REQUIRE(*index == 1);
  }

  SECTION("Поиск процесса по procState (не найден) [getIndexByState(processes, "
          "state)]") {
    pm::ProcessesState state{                         // processes
                             {pm::Process{}.pid(1),   //
                              pm::Process{}.pid(5),   //
                              pm::Process{}.pid(12)}, //
                                                      // queues
                             Queues{}};

    auto index = pm::getIndexByState(state, pm::ProcState::WAITING);

    REQUIRE(!index.has_value());
  }

  SECTION("Поиск процесса по state (не найден) [getIndexByState(processes, "
          "state)]") {
    pm::ProcessesState state{                         // processes
                             {pm::Process{}.pid(1),   //
                              pm::Process{}.pid(5),   //
                              pm::Process{}.pid(12)}, //
                                                      // queues
                             Queues{}};

    auto index = pm::getIndexByState(state.processes, pm::ProcState::WAITING);

    REQUIRE(!index.has_value());
  }
}
