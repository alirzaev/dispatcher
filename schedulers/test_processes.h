#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include <vector>

#include "algo/processes/requests.h"
#include "algo/processes/types.h"

using namespace ProcessesManagement;
using std::vector;

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

TEST_CASE("test_processes_types_process") {
  SECTION("Create valid instance of Process") {
    Process process;

    REQUIRE(process.pid() == 0);
    REQUIRE(process.ppid() == -1);
    REQUIRE(process.priority() == 0);
    REQUIRE(process.basePriority() == 0);
    REQUIRE(process.timer() == 0);
    REQUIRE(process.workTime() == 0);
    REQUIRE(process.state() == ProcessState::ACTIVE);
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
