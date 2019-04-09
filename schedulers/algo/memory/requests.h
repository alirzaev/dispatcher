#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <variant>

#include <nlohmann/json.hpp>

#include "exceptions.h"

namespace MemoryManagement {

/**
 *  Класс заявки на создание нового процесса.
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

  /**
   *  Возвращает заявку в виде JSON-объекта.
   */
  nlohmann::json dump() const {
    return {{"type", "CREATE_PROCESS"},
            {"pid", _pid},
            {"bytes", _bytes},
            {"pages", _pages}};
  }

  /**
   *  @brief Создает заявку на создание нового процесса.
   *
   *  @param pid Идентификатор процесса.
   *  @param bytes Количество байт для выделения процессу в памяти.
   *  @param pages Количества страниц для выделения процессу в памяти.
   *
   *  @throws MemoryManagement::RequestException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
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

/**
 *  Класс заявки на завершение существующего процесса.
 */
class TerminateProcessReq {
private:
  int32_t _pid;

public:
  int32_t pid() const { return _pid; }

  TerminateProcessReq &operator=(const TerminateProcessReq &rhs) = default;

  /**
   *  Возвращает заявку в виде JSON-объекта.
   */
  nlohmann::json dump() const {
    return {{"type", "TERMINATE_PROCESS"}, {"pid", _pid}};
  }

  /**
   *  @brief Создает заявку на завершение существующего процесса.
   *
   *  @param pid Идентификатор процесса.
   *
   *  @throws MemoryManagement::RequestException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  TerminateProcessReq(int32_t pid) : _pid(pid) {
    if (pid < -1 || pid > 255) {
      throw RequestException("INVALID_PID");
    }
  }
};

/**
 *  Класс заявки на выделение существующему процессу памяти.
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

  /**
   *  Возвращает заявку в виде JSON-объекта.
   */
  nlohmann::json dump() const {
    return {{"type", "ALLOCATE_MEMORY"},
            {"pid", _pid},
            {"bytes", _bytes},
            {"pages", _pages}};
  }

  /**
   *  @brief Создает заявку на выделение существующему процессу памяти.
   *
   *  @param pid Идентификатор процесса.
   *  @param bytes Количество байт для выделения процессу в памяти.
   *  @param pages Количества страниц для выделения процессу в памяти.
   *
   *  @throws MemoryManagement::RequestException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
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

/**
 *  Класс заявки на освобождение памяти.
 */
class FreeMemory {
private:
  int32_t _pid;

  int32_t _address;

public:
  int32_t pid() const { return _pid; }

  int32_t address() const { return _address; }

  FreeMemory &operator=(const FreeMemory &rhs) = default;

  /**
   *  Возвращает заявку в виде JSON-объекта.
   */
  nlohmann::json dump() const {
    return {{"type", "FREE_MEMORY"}, {"pid", _pid}, {"address", _address}};
  }

  /**
   *  @brief Создает заявку на освобождение памяти.
   *
   *  @param pid Идентификатор процесса.
   *  @param address Адрес начала блока памяти, который нужно освободить.
   *
   *  @throws MemoryManagement::RequestException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
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
