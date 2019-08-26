#include <array>
#include <deque>
#include <vector>

#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include <algo/processes/exceptions.h>
#include <algo/processes/operations.h>
#include <algo/processes/types.h>

namespace pm = ProcessesManagement;
using std::vector;
using Queues = std::array<std::deque<int32_t>, 16>;

TEST_CASE("ProcessesManagement::changeProcessState") {
  SECTION("Изменение состояния процесса") {
    auto state = pm::ProcessesState{
        // processes
        {pm::Process{}.pid(0).state(pm::ProcState::EXECUTING)},
        // queues
        {{0, {0}}}};

    auto expectedState =
        pm::ProcessesState{// processes
                           {pm::Process{}.pid(0).state(pm::ProcState::ACTIVE)},
                           // queues
                           {{0, {0}}}};

    auto actualState = changeProcessState(state, 0, pm::ProcState::ACTIVE);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Изменение состояния процесса (процесс не существует)") {
    auto state = pm::ProcessesState{// processes
                                    {},
                                    // queues
                                    Queues{}};

    REQUIRE_THROWS_AS(changeProcessState(state, 1, pm::ProcState::ACTIVE),
                      pm::OperationException);
  }
}

TEST_CASE("ProcessesManagement::pushToQueue") {
  SECTION("Добавить процесс в очередь") {
    auto state =
        pm::ProcessesState{// processes
                           {pm::Process{}.pid(0).state(pm::ProcState::ACTIVE)},
                           // queues
                           Queues{}};

    auto expectedState = pm::ProcessesState{
        // processes
        {pm::Process{}.pid(0).priority(1).state(pm::ProcState::ACTIVE)},
        // queues
        {{1, {0}}}};

    auto actualState = pushToQueue(state, 1, 0);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Добавить процесс в очередь (процесс не существует)") {
    auto state = pm::ProcessesState{// processes
                                    {},
                                    // queues
                                    Queues{}};

    REQUIRE_THROWS_AS(pushToQueue(state, 1, 1), pm::OperationException);
  }

  SECTION("Добавить процесс в очередь (процесс уже в очереди)") {
    auto state = pm::ProcessesState{
        // processes
        {pm::Process{}.pid(0).priority(1).state(pm::ProcState::ACTIVE)},
        // queues
        {{1, {0}}}};

    REQUIRE_THROWS_AS(pushToQueue(state, 1, 0), pm::OperationException);
  }
}

TEST_CASE("ProcessesManagement::popFromQueue") {
  SECTION("Извлечь процесс из очереди") {
    auto state = pm::ProcessesState{
        // processes
        {pm::Process{}.pid(0).priority(1).state(pm::ProcState::ACTIVE)},
        // queues
        {{1, {0}}}};

    auto expectedState = pm::ProcessesState{
        // processes
        {pm::Process{}.pid(0).priority(1).state(pm::ProcState::ACTIVE)},
        // queues
        Queues{}};

    auto actualState = popFromQueue(state, 1);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Извлечь процесс из очереди (очередь пуста)") {
    auto state = pm::ProcessesState{// processes
                                    {},
                                    // queues
                                    Queues{}};

    REQUIRE_THROWS_AS(popFromQueue(state, 0), pm::OperationException);
  }
}

TEST_CASE("ProcessesManagement::switchTo") {
  SECTION("Переключение на процесс") {
    auto state = pm::ProcessesState{
        // processes
        {pm::Process{}.pid(0).state(pm::ProcState::ACTIVE),
         pm::Process{}.pid(1).state(pm::ProcState::EXECUTING)},
        // queues
        Queues{}};

    auto expectedState = pm::ProcessesState{
        // processes
        {pm::Process{}.pid(0).state(pm::ProcState::EXECUTING),
         pm::Process{}.pid(1).state(pm::ProcState::ACTIVE)},
        // queues
        Queues{}};

    auto actualState = switchTo(state, 0);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Переключение на процесс (на тот же самый)") {
    auto state = pm::ProcessesState{
        // processes
        {pm::Process{}.pid(0).state(pm::ProcState::ACTIVE),
         pm::Process{}.pid(1).state(pm::ProcState::EXECUTING)},
        // queues
        Queues{}};

    auto expectedState = pm::ProcessesState{
        // processes
        {pm::Process{}.pid(0).state(pm::ProcState::ACTIVE),
         pm::Process{}.pid(1).state(pm::ProcState::EXECUTING)},
        // queues
        Queues{}};

    auto actualState = switchTo(state, 1);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Переключение на процесс (процесс не существует)") {
    auto state = pm::ProcessesState{
        // processes
        {pm::Process{}.pid(0).state(pm::ProcState::ACTIVE),
         pm::Process{}.pid(1).state(pm::ProcState::EXECUTING)},
        // queues
        Queues{}};

    REQUIRE_THROWS_AS(switchTo(state, 2), pm::OperationException);
  }

  SECTION("Переключение на процесс (процесс не ACTIVE)") {
    auto state = pm::ProcessesState{
        // processes
        {pm::Process{}.pid(0).state(pm::ProcState::WAITING),
         pm::Process{}.pid(1).state(pm::ProcState::EXECUTING)},
        // queues
        Queues{}};

    REQUIRE_THROWS_AS(switchTo(state, 0), pm::OperationException);
  }
}

TEST_CASE("ProcessesManagement::terminateProcess") {
  SECTION("Завершение процесса") {
    auto state = pm::ProcessesState{                               // processes
                                    {pm::Process{}.pid(0),         //
                                     pm::Process{}.pid(1).ppid(0), //
                                     pm::Process{}.pid(2).ppid(1), //
                                     pm::Process{}.pid(3),         //
                                     pm::Process{}.pid(4).ppid(3)},
                                    // queues
                                    {{0, {0, 1}}, //
                                     {1, {3, 4, 2}}}};

    auto expectedState = pm::ProcessesState{                       // processes
                                            {pm::Process{}.pid(3), //
                                             pm::Process{}.pid(4).ppid(3)},
                                            // queues
                                            {{1, {3, 4}}}};

    auto actualState = terminateProcess(state, 0);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Завершение процесса (процесс не существует)") {
    auto state = pm::ProcessesState{                               // processes
                                    {pm::Process{}.pid(0),         //
                                     pm::Process{}.pid(1).ppid(0), //
                                     pm::Process{}.pid(2).ppid(1), //
                                     pm::Process{}.pid(3),         //
                                     pm::Process{}.pid(4).ppid(3)},
                                    // queues
                                    {{0, {0, 1}}, //
                                     {1, {3, 4, 2}}}};

    REQUIRE_THROWS_AS(terminateProcess(state, 5), pm::OperationException);
  }
}

TEST_CASE("ProcessesManagement::addProcess") {
  SECTION("Добавление процесса") {
    auto state = pm::ProcessesState{                               // processes
                                    {pm::Process{}.pid(0),         //
                                     pm::Process{}.pid(1).ppid(0), //
                                     pm::Process{}.pid(2).ppid(1)},
                                    // queues
                                    Queues{}};

    auto expectedState = pm::ProcessesState{                       // processes
                                            {pm::Process{}.pid(0), //
                                             pm::Process{}.pid(1).ppid(0), //
                                             pm::Process{}.pid(2).ppid(1), //
                                             pm::Process{}.pid(3)},
                                            // queues
                                            Queues{}};

    auto actualState = addProcess(state, pm::Process{}.pid(3));
    REQUIRE(actualState == expectedState);
  }

  SECTION("Добавление процесса (процесс уже добавлен)") {
    auto state = pm::ProcessesState{                               // processes
                                    {pm::Process{}.pid(0),         //
                                     pm::Process{}.pid(1).ppid(0), //
                                     pm::Process{}.pid(2).ppid(1)},
                                    // queues
                                    Queues{}};

    REQUIRE_THROWS_AS(addProcess(state, pm::Process{}.pid(0)),
                      pm::OperationException);
  }

  SECTION("Добавление процесса (родительский процесс не существует)") {
    auto state = pm::ProcessesState{                               // processes
                                    {pm::Process{}.pid(0),         //
                                     pm::Process{}.pid(1).ppid(0), //
                                     pm::Process{}.pid(2).ppid(1)},
                                    // queues
                                    Queues{}};

    REQUIRE_THROWS_AS(addProcess(state, pm::Process{}.pid(3).ppid(4)),
                      pm::OperationException);
  }
}

TEST_CASE("ProcessesManagement::updateTimer") {
  SECTION("Обновление таймера") {
    auto state = pm::ProcessesState{
        // processes
        {pm::Process{}.pid(0).state(pm::ProcState::ACTIVE),
         pm::Process{}.pid(1).timer(0).state(pm::ProcState::EXECUTING)},
        // queues
        Queues{}};

    auto expectedState = pm::ProcessesState{
        // processes
        {pm::Process{}.pid(0).state(pm::ProcState::ACTIVE),
         pm::Process{}.pid(1).timer(1).state(pm::ProcState::EXECUTING)},
        // queues
        Queues{}};

    auto actualState = updateTimer(state);
    REQUIRE(actualState == expectedState);
  }
}
