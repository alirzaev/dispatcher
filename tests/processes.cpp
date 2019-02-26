#include <array>
#include <deque>
#include <vector>

#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include <algo/processes/exceptions.h>
#include <algo/processes/operations.h>
#include <algo/processes/requests.h>
#include <algo/processes/types.h>

using namespace ProcessesManagement;
using std::array;
using std::deque;
using std::vector;
using Queues = std::array<std::deque<int32_t>, 16>;

TEST_CASE("test_processes_requests") {
  SECTION("Create instance of CreateProcessReq") {
    auto request = ProcessesManagement::CreateProcessReq(0);

    REQUIRE(request.pid() == 0);
    REQUIRE(request.ppid() == -1);
    REQUIRE(request.priority() == 0);
    REQUIRE(request.basePriority() == 0);
    REQUIRE(request.timer() == 0);
    REQUIRE(request.workTime() == 0);
  }

  SECTION("Convert to JSON instance of CreateProcessReq") {
    auto request = ProcessesManagement::CreateProcessReq(0);

    auto expected = nlohmann::json{{"type", "CREATE_PROCESS"},
                                   {"pid", 0},
                                   {"ppid", -1},
                                   {"priority", 0},
                                   {"basePriority", 0},
                                   {"timer", 0},
                                   {"workTime", 0}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Check preconditions for CreateProcessReq") {
    REQUIRE_THROWS_AS(ProcessesManagement::CreateProcessReq(-1),
                      ProcessesManagement::RequestException);
    REQUIRE_THROWS_AS(ProcessesManagement::CreateProcessReq(0, -2),
                      ProcessesManagement::RequestException);

    REQUIRE_THROWS_AS(ProcessesManagement::CreateProcessReq(0, -1, -1),
                      ProcessesManagement::RequestException);
    REQUIRE_THROWS_AS(ProcessesManagement::CreateProcessReq(0, -1, 0, -1),
                      ProcessesManagement::RequestException);
    REQUIRE_THROWS_AS(ProcessesManagement::CreateProcessReq(0, -1, 1, 2),
                      ProcessesManagement::RequestException);
    REQUIRE_THROWS_AS(ProcessesManagement::CreateProcessReq(0, -1, 2, 2, -1),
                      ProcessesManagement::RequestException);

    REQUIRE_THROWS_AS(ProcessesManagement::CreateProcessReq(0, -1, 2, 2, 0, -1),
                      ProcessesManagement::RequestException);
  }

  SECTION("Create instance of TerminateProcessReq") {
    auto request = ProcessesManagement::TerminateProcessReq(0);

    REQUIRE(request.pid() == 0);
  }

  SECTION("Convert to JSON instance of TerminateProcessReq") {
    auto request = ProcessesManagement::TerminateProcessReq(0);

    auto expected = nlohmann::json{{"type", "TERMINATE_PROCESS"}, {"pid", 0}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Check preconditions for TerminateProcessReq") {
    REQUIRE_THROWS_AS(ProcessesManagement::TerminateProcessReq(-2),
                      ProcessesManagement::RequestException);
    REQUIRE_THROWS_AS(ProcessesManagement::TerminateProcessReq(256),
                      ProcessesManagement::RequestException);
  }

  SECTION("Create instance of InitIO") {
    auto request = ProcessesManagement::InitIO(0);

    REQUIRE(request.pid() == 0);
  }

  SECTION("Convert to JSON instance of InitIO") {
    auto request = ProcessesManagement::InitIO(0);

    auto expected = nlohmann::json{{"type", "INIT_IO"}, {"pid", 0}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Check preconditions for InitIO") {
    REQUIRE_THROWS_AS(ProcessesManagement::InitIO(-2),
                      ProcessesManagement::RequestException);
    REQUIRE_THROWS_AS(ProcessesManagement::InitIO(256),
                      ProcessesManagement::RequestException);
  }

  SECTION("Create instance of TerminateIO") {
    auto request = ProcessesManagement::TerminateIO(0);

    REQUIRE(request.pid() == 0);
  }

  SECTION("Convert to JSON instance of TerminateIO") {
    auto request = ProcessesManagement::TerminateIO(0);

    auto expected = nlohmann::json{{"type", "TERMINATE_IO"}, {"pid", 0}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Check preconditions for TerminateIO") {
    REQUIRE_THROWS_AS(ProcessesManagement::TerminateIO(-2),
                      ProcessesManagement::RequestException);
    REQUIRE_THROWS_AS(ProcessesManagement::TerminateIO(256),
                      ProcessesManagement::RequestException);
  }

  SECTION("Create instance of TransferControl") {
    auto request = ProcessesManagement::TransferControl(0);

    REQUIRE(request.pid() == 0);
  }

  SECTION("Convert to JSON instance of TransferControl") {
    auto request = ProcessesManagement::TransferControl(0);

    auto expected = nlohmann::json{{"type", "TRANSFER_CONTROL"}, {"pid", 0}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Check preconditions for TerminateIO") {
    REQUIRE_THROWS_AS(ProcessesManagement::TransferControl(-2),
                      ProcessesManagement::RequestException);
    REQUIRE_THROWS_AS(ProcessesManagement::TransferControl(256),
                      ProcessesManagement::RequestException);
  }

  SECTION("Convert to JSON instance of TimeQuantumExpired") {
    auto request = ProcessesManagement::TimeQuantumExpired();

    auto expected = nlohmann::json{{"type", "TIME_QUANTUM_EXPIRED"}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }
}

TEST_CASE("test_processes_operations") {
  SECTION("Change process state") {
    ProcessesState state{
        {Process{}.pid(0).state(ProcState::EXECUTING)}, // processes
        {}                                              // queues
    };
    ProcessesState expectedState{
        {Process{}.pid(0).state(ProcState::WAITING)}, // processes
        {}                                            // queues
    };

    auto actualState = changeProcessState(state, 0, ProcState::WAITING);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Change process state (invalid PID)") {
    ProcessesState state{
        {Process{}.pid(0).state(ProcState::EXECUTING)}, // processes
        {}                                              // queues
    };

    REQUIRE_THROWS_AS(changeProcessState(state, 1, ProcState::WAITING),
                      ProcessesManagement::OperationException);
  }

  SECTION("Change process state (the same state)") {
    ProcessesState state{
        {Process{}.pid(0).state(ProcState::EXECUTING)}, // processes
        {}                                              // queues
    };
    ProcessesState expectedState{
        {Process{}.pid(0).state(ProcState::EXECUTING)}, // processes
        {}                                              // queues
    };

    auto actualState = changeProcessState(state, 0, ProcState::EXECUTING);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Push to queue") {
    ProcessesState state{
        {Process{}.pid(0).state(ProcState::ACTIVE)}, // processes
        {}                                           // queues
    };

    Queues expectedQueues;
    expectedQueues[0].push_back(0);

    ProcessesState expectedState{
        {Process{}.pid(0).state(ProcState::ACTIVE)}, // processes
        expectedQueues                               // queues
    };

    auto actualState = pushToQueue(state, 0, 0);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Push to queue (invalid PID)") {
    ProcessesState state{
        {Process{}.pid(0).state(ProcState::ACTIVE)}, // processes
        {}                                           // queues
    };

    REQUIRE_THROWS_AS(pushToQueue(state, 0, 1),
                      ProcessesManagement::OperationException);
  }

  SECTION("Push to queue (process is already in queue)") {
    Queues queues;
    queues[0].push_back(0);
    ProcessesState state{
        {Process{}.pid(0).state(ProcState::ACTIVE)}, // processes
        queues                                       // queues
    };

    REQUIRE_THROWS_AS(pushToQueue(state, 0, 0),
                      ProcessesManagement::OperationException);
  }

  SECTION("Pop from queue") {
    Queues queues;
    queues[0].push_back(0);
    ProcessesState state{
        {Process{}.pid(0).state(ProcState::ACTIVE)}, // processes
        queues                                       // queues
    };

    ProcessesState expectedState{
        {Process{}.pid(0).state(ProcState::ACTIVE)}, // processes
        {}                                           // queues
    };

    auto actualState = popFromQueue(state, 0);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Pop from queue (empty queue)") {
    ProcessesState state{
        {Process{}.pid(0).state(ProcState::ACTIVE)}, // processes
        {}                                           // queues
    };

    REQUIRE_THROWS_AS(popFromQueue(state, 0),
                      ProcessesManagement::OperationException);
  }

  SECTION("Switch to process") {
    ProcessesState state{
        {Process{}.pid(0).state(ProcState::ACTIVE),
         Process{}.pid(1).state(ProcState::EXECUTING)}, // processes
        {}                                              // queues
    };

    ProcessesState expectedState{
        {Process{}.pid(0).state(ProcState::EXECUTING),
         Process{}.pid(1).state(ProcState::ACTIVE)}, // processes
        {}                                           // queues
    };

    auto actualState = switchTo(state, 0);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Switch to process (the same process)") {
    ProcessesState state{
        {Process{}.pid(0).state(ProcState::ACTIVE),
         Process{}.pid(1).state(ProcState::EXECUTING)}, // processes
        {}                                              // queues
    };

    ProcessesState expectedState{
        {Process{}.pid(0).state(ProcState::ACTIVE),
         Process{}.pid(1).state(ProcState::EXECUTING)}, // processes
        {}                                              // queues
    };

    auto actualState = switchTo(state, 1);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Switch to process (invalid PID)") {
    ProcessesState state{
        {Process{}.pid(0).state(ProcState::ACTIVE),
         Process{}.pid(1).state(ProcState::EXECUTING)}, // processes
        {}                                              // queues
    };

    REQUIRE_THROWS_AS(switchTo(state, 2),
                      ProcessesManagement::OperationException);
  }

  SECTION("Switch to process (invalid proc state)") {
    ProcessesState state{
        {Process{}.pid(0).state(ProcState::WAITING),
         Process{}.pid(1).state(ProcState::EXECUTING)}, // processes
        {}                                              // queues
    };

    REQUIRE_THROWS_AS(switchTo(state, 0),
                      ProcessesManagement::OperationException);
  }

  SECTION("Terminate process") {
    Queues queues;
    queues[0] = {0, 1};
    queues[1] = {3, 4, 2};
    ProcessesState state{
        {Process{}.pid(0),          //
         Process{}.pid(1).ppid(0),  //
         Process{}.pid(2).ppid(1),  //
         Process{}.pid(3),          //
         Process{}.pid(4).ppid(3)}, // processes
        queues                      // queues
    };

    Queues expecteQueues;
    expecteQueues[1] = {3, 4};
    ProcessesState expectedState{
        {Process{}.pid(3),          //
         Process{}.pid(4).ppid(3)}, // processes
        expecteQueues               // queues // queues
    };

    auto actualState = terminateProcess(state, 0);
    REQUIRE(actualState == expectedState);
  }

  SECTION("Terminate process (invalid PID)") {
    Queues queues;
    queues[0] = {0, 1};
    queues[1] = {3, 4, 2};
    ProcessesState state{
        {Process{}.pid(0),          //
         Process{}.pid(1).ppid(0),  //
         Process{}.pid(2).ppid(1),  //
         Process{}.pid(3),          //
         Process{}.pid(4).ppid(3)}, // processes
        queues                      // queues
    };

    REQUIRE_THROWS_AS(terminateProcess(state, 5),
                      ProcessesManagement::OperationException);
  }

  SECTION("Add process") {
    ProcessesState state{
        {Process{}.pid(0),          //
         Process{}.pid(1).ppid(0),  //
         Process{}.pid(2).ppid(1)}, // processes
        {}                          // queues
    };

    ProcessesState expectedState{
        {Process{}.pid(0),         //
         Process{}.pid(1).ppid(0), //
         Process{}.pid(2).ppid(1), //
         Process{}.pid(3)},        // processes
        {}                         // queues
    };

    auto actualState = addProcess(state, Process{}.pid(3));
    REQUIRE(actualState == expectedState);
  }

  SECTION("Add process (process already exists)") {
    ProcessesState state{
        {Process{}.pid(0),          //
         Process{}.pid(1).ppid(0),  //
         Process{}.pid(2).ppid(1)}, // processes
        {}                          // queues
    };

    ProcessesState expectedState{
        {Process{}.pid(0),          //
         Process{}.pid(1).ppid(0),  //
         Process{}.pid(2).ppid(1)}, // processes
        {}                          // queues
    };

    REQUIRE_THROWS_AS(addProcess(state, Process{}.pid(0)),
                      ProcessesManagement::OperationException);
  }
}

TEST_CASE("test_processes_types_process") {
  SECTION("Create valid instance of Process") {
    Process process;

    REQUIRE(process.pid() == 0);
    REQUIRE(process.ppid() == -1);
    REQUIRE(process.priority() == 0);
    REQUIRE(process.basePriority() == 0);
    REQUIRE(process.timer() == 0);
    REQUIRE(process.workTime() == 0);
    REQUIRE(process.state() == ProcState::ACTIVE);
  }

  SECTION("Check preconditions for Process") {
    // Invalid PID
    REQUIRE_THROWS_AS(Process().pid(-2), ProcessesManagement::TypeException);
    REQUIRE_THROWS_AS(Process().pid(256), ProcessesManagement::TypeException);

    // Invalid PPID
    REQUIRE_THROWS_AS(Process().ppid(-2), ProcessesManagement::TypeException);
    REQUIRE_THROWS_AS(Process().ppid(256), ProcessesManagement::TypeException);

    // Invalid priority
    REQUIRE_THROWS_AS(Process().priority(-1),
                      ProcessesManagement::TypeException);
    REQUIRE_THROWS_AS(Process().priority(16),
                      ProcessesManagement::TypeException);

    // Invalid base priority
    REQUIRE_THROWS_AS(Process().basePriority(-1),
                      ProcessesManagement::TypeException);
    REQUIRE_THROWS_AS(Process().basePriority(16),
                      ProcessesManagement::TypeException);
    REQUIRE_THROWS_AS(Process().basePriority(1),
                      ProcessesManagement::TypeException);

    // Invalid times
    REQUIRE_THROWS_AS(Process().timer(-1), ProcessesManagement::TypeException);
    REQUIRE_THROWS_AS(Process().workTime(-1),
                      ProcessesManagement::TypeException);
  }

  SECTION("Convert to JSON instance of Process") {
    auto process = ProcessesManagement::Process();

    auto expected = nlohmann::json{
        {"pid", 0},   {"ppid", -1},    {"priority", 0},    {"basePriority", 0},
        {"timer", 0}, {"workTime", 0}, {"state", "ACTIVE"}};
    auto actual = process.dump();

    REQUIRE(actual == expected);
  }
}
