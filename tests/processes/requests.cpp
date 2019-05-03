#include <array>
#include <deque>
#include <vector>

#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include <algo/processes/exceptions.h>
#include <algo/processes/requests.h>
#include <algo/processes/types.h>

namespace pm = ProcessesManagement;

TEST_CASE("ProcessesManagement::CreateProcessReq") {
  SECTION("Создать экземпляр CreateProcessReq") {
    auto request = pm::CreateProcessReq(0);

    REQUIRE(request.pid() == 0);
    REQUIRE(request.ppid() == -1);
    REQUIRE(request.priority() == 0);
    REQUIRE(request.basePriority() == 0);
    REQUIRE(request.timer() == 0);
    REQUIRE(request.workTime() == 0);
  }

  SECTION("Получить JSON экземпляра CreateProcessReq") {
    auto request = pm::CreateProcessReq(0);

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

  SECTION("Ограничения на параметры CreateProcessReq") {
    // Некорректный PID
    REQUIRE_THROWS_AS(pm::CreateProcessReq(-1), pm::RequestException);
    REQUIRE_THROWS_AS(pm::CreateProcessReq(256), pm::RequestException);

    // Некорректный PPID
    REQUIRE_THROWS_AS(pm::CreateProcessReq(0, -2), pm::RequestException);
    REQUIRE_THROWS_AS(pm::CreateProcessReq(0, 256), pm::RequestException);

    // Некорректный приоритет
    REQUIRE_THROWS_AS(pm::CreateProcessReq(0, -1, -1), pm::RequestException);
    REQUIRE_THROWS_AS(pm::CreateProcessReq(0, -1, 16), pm::RequestException);

    // Некорректный базовый приоритет
    REQUIRE_THROWS_AS(pm::CreateProcessReq(0, -1, 0, -1), pm::RequestException);
    REQUIRE_THROWS_AS(pm::CreateProcessReq(0, -1, 0, 16), pm::RequestException);

    // Текущий приоритет меньше базового
    REQUIRE_THROWS_AS(pm::CreateProcessReq(0, -1, 0, 1), pm::RequestException);

    // Некорректное время работы
    REQUIRE_THROWS_AS(pm::CreateProcessReq(0, -1, 0, 0, -1),
                      pm::RequestException);

    // Некорректное заявленное время работы
    REQUIRE_THROWS_AS(pm::CreateProcessReq(0, -1, 0, 0, 0, -1),
                      pm::RequestException);
  }

  SECTION("Получение экземпляра Process") {
    auto request = pm::CreateProcessReq(0, -1, 0, 0, 0, 0);

    auto process = request.toProcess();

    REQUIRE(process.pid() == 0);
    REQUIRE(process.ppid() == -1);
    REQUIRE(process.priority() == 0);
    REQUIRE(process.basePriority() == 0);
    REQUIRE(process.timer() == 0);
    REQUIRE(process.workTime() == 0);
  }
}

TEST_CASE("ProcessesManagement::TerminateProcessReq") {
  SECTION("Создать экземпляр TerminateProcessReq") {
    auto request = pm::TerminateProcessReq(0);

    REQUIRE(request.pid() == 0);
  }

  SECTION("Получить JSON экземпляра TerminateProcessReq") {
    auto request = pm::TerminateProcessReq(0);

    auto expected = nlohmann::json{{"type", "TERMINATE_PROCESS"}, //
                                   {"pid", 0}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Ограничения на параметры TerminateProcessReq") {
    // Некорректный PID
    REQUIRE_THROWS_AS(pm::TerminateProcessReq(-1), pm::RequestException);
    REQUIRE_THROWS_AS(pm::TerminateProcessReq(256), pm::RequestException);
  }
}

TEST_CASE("ProcessesManagement::InitIO") {
  SECTION("Создать экземпляр InitIO") {
    auto request = pm::InitIO(0);

    REQUIRE(request.pid() == 0);
  }

  SECTION("Получить JSON экземпляра InitIO") {
    auto request = pm::InitIO(0);

    auto expected = nlohmann::json{{"type", "INIT_IO"}, //
                                   {"pid", 0}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Ограничения на параметры InitIO") {
    // Некорректный PID
    REQUIRE_THROWS_AS(pm::InitIO(-1), pm::RequestException);
    REQUIRE_THROWS_AS(pm::InitIO(256), pm::RequestException);
  }
}

TEST_CASE("ProcessesManagement::TerminateIO") {
  SECTION("Создать экземпляр TerminateIO") {
    auto request = pm::TerminateIO(0);

    REQUIRE(request.pid() == 0);
  }

  SECTION("Получить JSON экземпляра TerminateIO") {
    auto request = pm::TerminateIO(0);

    auto expected = nlohmann::json{{"type", "TERMINATE_IO"}, //
                                   {"pid", 0}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Ограничения на параметры TerminateIO") {
    // Некорректный PID
    REQUIRE_THROWS_AS(pm::TerminateIO(-1), pm::RequestException);
    REQUIRE_THROWS_AS(pm::TerminateIO(256), pm::RequestException);
  }
}

TEST_CASE("ProcessesManagement::TransferControl") {
  SECTION("Создать экземпляр TransferControl") {
    auto request = pm::TransferControl(0);

    REQUIRE(request.pid() == 0);
  }

  SECTION("Получить JSON экземпляра TransferControl") {
    auto request = pm::TransferControl(0);

    auto expected = nlohmann::json{{"type", "TRANSFER_CONTROL"}, //
                                   {"pid", 0}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Ограничения на параметры TransferControl") {
    // Некорректный PID
    REQUIRE_THROWS_AS(pm::TransferControl(-1), pm::RequestException);
    REQUIRE_THROWS_AS(pm::TransferControl(256), pm::RequestException);
  }
}

TEST_CASE("ProcessesManagement::TimeQuantumExpired") {
  SECTION("Получить JSON экземпляра TimeQuantumExpired") {
    auto request = pm::TimeQuantumExpired();

    auto expected = nlohmann::json{{"type", "TIME_QUANTUM_EXPIRED"}};

    auto actual = request.dump();

    REQUIRE(actual == expected);
  }
}
