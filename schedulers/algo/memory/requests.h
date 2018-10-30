#pragma once

#include "exceptions.h"
#include <cstdint>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>

namespace MemoryManagement::Requests {
using std::shared_ptr;

enum class RequestType {
  CREATE_PROCESS,
  TERMINATE_PROCESS,
  ALLOCATE_MEMORY,
  FREE_MEMORY
};

class AbstractRequest {
public:
  AbstractRequest(RequestType type) : type(type) {}

  const RequestType type;

  virtual std::string toString() const = 0;

  virtual nlohmann::json dump() const = 0;

  virtual ~AbstractRequest() = default;
};

using RequestPtr = shared_ptr<AbstractRequest>;

/*
 * Класс заявки на создание нового процесса
 */
class CreateProcess final : public AbstractRequest {
public:
  const int32_t pid;

  const int32_t bytes;

  const int32_t pages;

  std::string toString() const override { return "CreateProcess"; }

  CreateProcess &operator=(const CreateProcess &rhs) = default;

  /*
   * Создание заявки на создание нового процесса
   *
   * @param pid идентификатор процесса
   * @param bytes количество байт для выделения процессу в памяти
   * @param pages количества страниц для выделения процессу в памяти
   *
   * @return экземпляр заявки
   */
  static shared_ptr<CreateProcess> create(int32_t pid, int32_t bytes,
                                          int32_t pages) {
    if (pid < -1 || pid > 255) {
      throw Exceptions::RequestException("INVALID_PID");
    }
    if (bytes < 1 || bytes > 256 * 4096) {
      throw Exceptions::RequestException("INVALID_BYTES");
    }
    if (pages < 1 || pages > 256) {
      throw Exceptions::RequestException("INVALID_PAGES");
    }
    if (bytes <= (pages - 1) * 4096 || bytes > pages * 4096) {
      throw Exceptions::RequestException("INVALID_BYTES");
    }

    return shared_ptr<CreateProcess>(new CreateProcess(pid, bytes, pages));
  }

  nlohmann::json dump() const override {
    return {{"type", "CREATE_PROCESS"},
            {"pid", pid},
            {"bytes", bytes},
            {"pages", pages}};
  }

private:
  CreateProcess(int32_t pid, int32_t bytes, int32_t pages)
      : AbstractRequest(RequestType::CREATE_PROCESS), pid(pid), bytes(bytes),
        pages(pages) {}
};

/*
 * Класс заявки на завершение существующего процесса
 */
class TerminateProcess final : public AbstractRequest {
public:
  const int32_t pid;

  std::string toString() const override { return "TerminateProcess"; }

  TerminateProcess &operator=(const TerminateProcess &rhs) = default;

  /*
   * Создание заявки на завершение существующего процесса
   *
   * @param pid идентификатор процесса
   *
   * @return экземпляр заявки
   */
  static shared_ptr<TerminateProcess> create(int32_t pid) {
    if (pid < -1 || pid > 255) {
      throw Exceptions::RequestException("INVALID_PID");
    }

    return shared_ptr<TerminateProcess>(new TerminateProcess(pid));
  }

  nlohmann::json dump() const override {
    return {{"type", "TERMINATE_PROCESS"}, {"pid", pid}};
  }

private:
  TerminateProcess(int32_t pid)
      : AbstractRequest(RequestType::TERMINATE_PROCESS), pid(pid) {}
};

/*
 * Класс заявки на выделение существующему процессу памяти
 */
class AllocateMemory final : public AbstractRequest {
public:
  const int32_t pid;

  const int32_t bytes;

  const int32_t pages;

  std::string toString() const override { return "AllocateMemory"; }

  AllocateMemory &operator=(const AllocateMemory &rhs) = default;

  /*
   * Создание заявки на выделение существующему процессу памяти
   *
   * @param pid идентификатор процесса
   * @param bytes количество байт для выделения процессу в памяти
   * @param pages количества страниц для выделения процессу в памяти
   *
   * @return экземпляр заявки
   */
  static shared_ptr<AllocateMemory> create(int32_t pid, int32_t bytes,
                                           int32_t pages) {
    if (pid < -1 || pid > 255) {
      throw Exceptions::RequestException("INVALID_PID");
    }
    if (bytes < 1 || bytes > 256 * 4096) {
      throw Exceptions::RequestException("INVALID_BYTES");
    }
    if (pages < 1 || pages > 256) {
      throw Exceptions::RequestException("INVALID_PAGES");
    }
    if (bytes <= (pages - 1) * 4096 || bytes > pages * 4096) {
      throw Exceptions::RequestException("INVALID_BYTES");
    }

    return shared_ptr<AllocateMemory>(new AllocateMemory(pid, bytes, pages));
  }

  nlohmann::json dump() const override {
    return {{"type", "ALLOCATE_MEMORY"},
            {"pid", pid},
            {"bytes", bytes},
            {"pages", pages}};
  }

private:
  AllocateMemory(int32_t pid, int32_t bytes, int32_t pages)
      : AbstractRequest(RequestType::ALLOCATE_MEMORY), pid(pid), bytes(bytes),
        pages(pages) {}
};

/*
 * Класс заявки на освобождение памяти
 */
class FreeMemory final : public AbstractRequest {
public:
  const int32_t pid;

  const int32_t address;

  std::string toString() const override { return "FreeMemory"; }

  FreeMemory &operator=(const FreeMemory &rhs) = default;

  /*
   * Создание заявки на освобождение памяти
   *
   * @param pid идентификатор процесса
   * @param address адрес начала блока памяти, который нужно освободить
   *
   * @return экземпляр заявки
   */
  static shared_ptr<FreeMemory> create(int32_t pid, int32_t address) {
    if (pid < -1 || pid > 255) {
      throw Exceptions::RequestException("INVALID_PID");
    }
    if (address < 0 || address > 255) {
      throw Exceptions::RequestException("INVALID_ADDRESS");
    }

    return shared_ptr<FreeMemory>(new FreeMemory(pid, address));
  }

  nlohmann::json dump() const override {
    return {{"type", "FREE_MEMORY"}, {"pid", pid}, {"address", address}};
  }

private:
  FreeMemory(int32_t pid, int32_t address)
      : AbstractRequest(RequestType::FREE_MEMORY), pid(pid), address(address) {}
};
} // namespace MemoryManagement::Requests
