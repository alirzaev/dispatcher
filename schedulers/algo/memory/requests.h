#pragma once

#include "exceptions.h"
#include <cstdint>
#include <memory>
#include <string>
#include <variant>

#include <nlohmann/json.hpp>

namespace MemoryManagement {

/*
 * Класс заявки на создание нового процесса
 */
class CreateProcessReq {
private:
  int32_t _pid;

  int32_t _bytes;

  int32_t _pages;

public:
  CreateProcessReq &operator=(const CreateProcessReq &rhs) = default;

  int32_t pid() const { return _pid; }

  int32_t bytes() const { return _bytes; }

  int32_t pages() const { return _pages; }

  nlohmann::json dump() const {
    return {{"type", "CREATE_PROCESS"},
            {"pid", _pid},
            {"bytes", _bytes},
            {"pages", _pages}};
  }

  /*
   * Заявка на создание нового процесса
   *
   * @param pid идентификатор процесса
   * @param bytes количество байт для выделения процессу в памяти
   * @param pages количества страниц для выделения процессу в памяти
   *
   */
  CreateProcessReq(int32_t pid, int32_t bytes, int32_t pages)
      : _pid(pid), _bytes(bytes), _pages(pages) {
    if (pid < -1 || pid > 255) {
      throw RequestException("INVALID_PID");
    }
    if (bytes < 1 || bytes > 256 * 4096) {
      throw RequestException("INVALID_BYTES");
    }
    if (pages < 1 || pages > 256) {
      throw RequestException("INVALID_PAGES");
    }
    if (bytes <= (pages - 1) * 4096 || bytes > pages * 4096) {
      throw RequestException("INVALID_BYTES");
    }
  }
};

/*
 * Класс заявки на завершение существующего процесса
 */
class TerminateProcessReq {
private:
  int32_t _pid;

public:
  int32_t pid() const { return _pid; }

  TerminateProcessReq &operator=(const TerminateProcessReq &rhs) = default;

  nlohmann::json dump() const {
    return {{"type", "TERMINATE_PROCESS"}, {"pid", _pid}};
  }

  /*
   * Заявка на на завершение существующего процесса
   *
   * @param pid идентификатор процесса
   *
   */
  TerminateProcessReq(int32_t pid) : _pid(pid) {
    if (pid < -1 || pid > 255) {
      throw RequestException("INVALID_PID");
    }
  }
};

/*
 * Класс заявки на выделение существующему процессу памяти
 */
class AllocateMemory {
private:
  int32_t _pid;

  int32_t _bytes;

  int32_t _pages;

public:
  int32_t pid() const { return _pid; }

  int32_t bytes() const { return _bytes; }

  int32_t pages() const { return _pages; }

  AllocateMemory &operator=(const AllocateMemory &rhs) = default;

  nlohmann::json dump() const {
    return {{"type", "ALLOCATE_MEMORY"},
            {"pid", _pid},
            {"bytes", _bytes},
            {"pages", _pages}};
  }

  /*
   * Заявка на выделение существующему процессу памяти
   *
   * @param pid идентификатор процесса
   * @param bytes количество байт для выделения процессу в памяти
   * @param pages количества страниц для выделения процессу в памяти
   *
   */
  AllocateMemory(int32_t pid, int32_t bytes, int32_t pages)
      : _pid(pid), _bytes(bytes), _pages(pages) {
    if (pid < -1 || pid > 255) {
      throw RequestException("INVALID_PID");
    }
    if (bytes < 1 || bytes > 256 * 4096) {
      throw RequestException("INVALID_BYTES");
    }
    if (pages < 1 || pages > 256) {
      throw RequestException("INVALID_PAGES");
    }
    if (bytes <= (pages - 1) * 4096 || bytes > pages * 4096) {
      throw RequestException("INVALID_BYTES");
    }
  }
};

/*
 * Класс заявки на освобождение памяти
 */
class FreeMemory {
private:
  int32_t _pid;

  int32_t _address;

public:
  int32_t pid() const { return _pid; }

  int32_t address() const { return _address; }

  FreeMemory &operator=(const FreeMemory &rhs) = default;

  nlohmann::json dump() const {
    return {{"type", "FREE_MEMORY"}, {"pid", _pid}, {"address", _address}};
  }

  /*
   * Заявка на освобождение памяти
   *
   * @param pid идентификатор процесса
   * @param address адрес начала блока памяти, который нужно освободить
   *
   */
  FreeMemory(int32_t pid, int32_t address) : _pid(pid), _address(address) {
    if (pid < -1 || pid > 255) {
      throw RequestException("INVALID_PID");
    }
    if (address < 0 || address > 255) {
      throw RequestException("INVALID_ADDRESS");
    }
  }
};

using Request = std::variant<CreateProcessReq, TerminateProcessReq,
                             AllocateMemory, FreeMemory>;
} // namespace MemoryManagement
