#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include <algo/memory/exceptions.h>
#include <algo/memory/requests.h>

namespace mm = MemoryManagement;

TEST_CASE("MemoryManagement::CreateProcessReq") {
  SECTION("Создать экземпляр CreateProcessReq") {
    auto request = mm::CreateProcessReq(0, 4096);

    REQUIRE(request.pid() == 0);
    REQUIRE(request.bytes() == 4096);
    REQUIRE(request.pages() == 1);
  }

  SECTION("Получить JSON экземпляра CreateProcessReq") {
    auto request = mm::CreateProcessReq(0, 4096);

    auto expected = nlohmann::json{{"type", "CREATE_PROCESS"}, //
                                   {"pid", 0},                 //
                                   {"bytes", 4096}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Ограничения на параметры CreateProcessReq") {
    // Некорректный PID
    REQUIRE_THROWS_AS(mm::CreateProcessReq(-1, 4096), mm::RequestException);
    REQUIRE_THROWS_AS(mm::CreateProcessReq(256, 4096), mm::RequestException);

    // Некорректный размер памяти в байтах
    REQUIRE_THROWS_AS(mm::CreateProcessReq(0, -1), mm::RequestException);
    REQUIRE_THROWS_AS(mm::CreateProcessReq(0, 257 * 4096),
                      mm::RequestException);
  }
}

TEST_CASE("MemoryManagement::TerminateProcessReq") {
  SECTION("Создать экземпляр TerminateProcessReq") {
    auto request = mm::TerminateProcessReq(0);

    REQUIRE(request.pid() == 0);
  }

  SECTION("Получить JSON экземпляра TerminateProcessReq") {
    auto request = mm::TerminateProcessReq(0);

    auto expected = nlohmann::json{{"type", "TERMINATE_PROCESS"}, //
                                   {"pid", 0}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Ограничения на параметры TerminateProcessReq") {
    // Некорректный PID
    REQUIRE_THROWS_AS(mm::TerminateProcessReq(-1), mm::RequestException);
    REQUIRE_THROWS_AS(mm::TerminateProcessReq(256), mm::RequestException);
  }
}

TEST_CASE("MemoryManagement::AllocateMemory") {
  SECTION("Создать экземпляр AllocateMemory") {
    auto request = mm::AllocateMemory(0, 4096);

    REQUIRE(request.pid() == 0);
    REQUIRE(request.bytes() == 4096);
    REQUIRE(request.pages() == 1);
  }

  SECTION("Получить JSON экземпляра AllocateMemory") {
    auto request = mm::AllocateMemory(0, 4096);

    auto expected = nlohmann::json{{"type", "ALLOCATE_MEMORY"}, //
                                   {"pid", 0},                  //
                                   {"bytes", 4096}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Ограничения на параметры AllocateMemory") {
    // Некорректный PID
    REQUIRE_THROWS_AS(mm::AllocateMemory(-1, 4096), mm::RequestException);
    REQUIRE_THROWS_AS(mm::AllocateMemory(256, 4096), mm::RequestException);

    // Некорректный размер памяти в байтах
    REQUIRE_THROWS_AS(mm::AllocateMemory(0, -1), mm::RequestException);
    REQUIRE_THROWS_AS(mm::AllocateMemory(0, 257 * 4096), mm::RequestException);
  }
}

TEST_CASE("MemoryManagement::FreeMemory") {
  SECTION("Создать экземпляр FreeMemory") {
    auto request = mm::FreeMemory(0, 12);

    REQUIRE(request.pid() == 0);
    REQUIRE(request.address() == 12);
  }

  SECTION("Получить JSON экземпляра") {
    auto request = mm::FreeMemory(0, 12);

    auto expected = nlohmann::json{{"type", "FREE_MEMORY"}, //
                                   {"pid", 0},              //
                                   {"address", 12}};
    auto actual = request.dump();

    REQUIRE(actual == expected);
  }

  SECTION("Ограничения на параметры FreeMemory") {
    // Некорректный PID
    REQUIRE_THROWS_AS(mm::FreeMemory(-1, 0), mm::RequestException);
    REQUIRE_THROWS_AS(mm::FreeMemory(256, 0), mm::RequestException);

    // Некорректный адрес
    REQUIRE_THROWS_AS(mm::FreeMemory(0, -1), mm::RequestException);
    REQUIRE_THROWS_AS(mm::FreeMemory(0, 256), mm::RequestException);
  }
}
