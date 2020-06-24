#pragma once

#include <algorithm>
#include <cstdint>
#include <set>
#include <tuple>
#include <vector>

#include <nlohmann/json.hpp>

#include "exceptions.h"

namespace MemoryManagement {
/**
 *  @brief Дескриптор блока памяти.
 *
 *  Дескриптор блока памяти - непрерывная область памяти, состоящая из одной и
 *  более страниц и имеющая начальный адрес и размер.
 */
class MemoryBlock {
private:
  int32_t _pid;

  int32_t _address;

  int32_t _size;

public:
  int32_t pid() const { return _pid; }

  int32_t address() const { return _address; }

  int32_t size() const { return _size; }

  /**
   *  @brief Создает дескриптор блока памяти с заданными параметрами.
   *
   *  @param pid Идентификатор процесса, которому выделен данный блок, или -1.
   *  @param address Адрес начала блока (в страницах).
   *  @param size Размер блока (в страницах).
   */
  MemoryBlock(int32_t pid, int32_t address, int32_t size)
      : _pid(pid), _address(address), _size(size) {
    validate(pid, address, size);
  }

  MemoryBlock(const MemoryBlock &other) = default;

  MemoryBlock() : MemoryBlock(-1, 0, 256) {}

  MemoryBlock &operator=(const MemoryBlock &rhs) = default;

  bool operator==(const MemoryBlock &rhs) const {
    return _pid == rhs._pid && _address == rhs._address && _size == rhs._size;
  }

  bool operator<(const MemoryBlock &rhs) const {
    return std::tuple{pid(), address(), size()} <
           std::tuple{rhs.pid(), rhs.address(), rhs.size()};
  }

  /**
   *  Возвращает дескриптор в виде JSON-объекта.
   */
  nlohmann::json dump() const {
    return {{"pid", _pid}, {"address", _address}, {"size", _size}};
  }

  /**
   *  @brief Проверяет параметры конструктора.
   *
   *  @param pid Идентификатор процесса, которому выделен данный блок, или -1.
   *  @param address Адрес начала блока (в страницах).
   *  @param size Размер блока (в страницах).
   *
   *  @throws MemoryManagement::TypeException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  static void validate(int32_t pid, int32_t address, int32_t size) {
    if (pid < -1 || pid > 255) {
      throw TypeException("INVALID_PID");
    }
    if (address < 0 || address > 255) {
      throw TypeException("INVALID_ADDRESS");
    }
    if (size < 1 || size > 256) {
      throw TypeException("INVALID_SIZE");
    }
    if (address + size > 256) {
      throw TypeException("OUT_OF_BOUNDS");
    }
  }
};

/**
 *  @brief Дескриптор состояния памяти.
 *
 *  В данной модели адресное пространство разбито на 256 страниц по 4096 байт.
 *  Наименьшая единица адресного пространства, доступная для выделения процессу
 *  - страница. Выделять можно только целое число страниц.
 *
 *  Процессам в данной модели могут присваиваться идентификаторы (PID) от 0 до
 *  255 включительно.
 *
 *  Непрерывная область памяти, состоящая из одной и более страниц и имеющая
 *  начальный адрес и размер, называется блоком памяти.
 *
 *  Состояние памяти определяется совокупностью всех блоков памяти. Каждая
 *  страница адресного пространства должна находиться в одном и только одном
 *  блоке памяти, т.е. последовательность блоков памяти, упорядоченных по
 *  начальному адресу, должна полностью покрыть адресное пространство.
 *
 *  Под начальным состоянием памяти подразумевается такое состояние памяти, при
 *  котором все адресное пространство покрыто одним свободным блоком памяти с
 *  начальным адресом 0 и размером 256.
 *
 *  Программная модель разработана исходя из того, что перед обработкой первой
 *  заявки память находится в начальном состоянии.
 */
class MemoryState {
public:
  std::vector<MemoryBlock> blocks;

  std::vector<MemoryBlock> freeBlocks;

  /**
   *  @brief Создает дескриптор состояния памяти с заданными параметрами.
   *
   *  @param blocks Массив из дескрипторов всех доступных блоков памяти.
   *  @param freeBlocks Массив из дескрипторов свободных блоков памяти,
   *  упорядоченных согласно стратегии.
   */
  MemoryState(const std::vector<MemoryBlock> &blocks,
              const std::vector<MemoryBlock> &freeBlocks)
      : blocks(blocks), freeBlocks(freeBlocks) {}

  MemoryState() : MemoryState(MemoryState::initial()) {}

  MemoryState(const MemoryState &state) = default;

  MemoryState(MemoryState &&state) = default;

  MemoryState &operator=(const MemoryState &state) = default;

  MemoryState &operator=(MemoryState &&state) = default;

  bool operator==(const MemoryState &state) const {
    return blocks == state.blocks && freeBlocks == state.freeBlocks;
  }

  bool operator!=(const MemoryState &state) const { return !(*this == state); }

  /**
   *  Возвращает дескриптор в виде JSON-объекта.
   */
  nlohmann::json dump() const {
    auto jsonBlocks = nlohmann::json::array();
    auto jsonFreeBlocks = nlohmann::json::array();

    for (const auto &block : blocks) {
      jsonBlocks.push_back(block.dump());
    }
    for (const auto &block : freeBlocks) {
      jsonFreeBlocks.push_back(block.dump());
    }

    return {{"blocks", jsonBlocks}, {"free_blocks", jsonFreeBlocks}};
  }

  /**
   *  Возвращает дескриптор с начальным состоянием.
   */
  static MemoryState initial() {
    return {{MemoryBlock{-1, 0, 256}}, {MemoryBlock{-1, 0, 256}}};
  }

  /**
   *  @brief Проверяет параметры конструктора.
   *
   *  @param blocks Массив из дескрипторов всех доступных блоков памяти.
   *  @param freeBlocks Массив из дескрипторов свободных блоков памяти,
   *  упорядоченных согласно стратегии.
   *
   *  @throws MemoryManagement::TypeException Исключение возникает, если
   *  переданные параметры не соответствуют заданным ограничениям.
   */
  static void validate(const std::vector<MemoryBlock> &blocks,
                       const std::vector<MemoryBlock> &freeBlocks) {
    if (blocks.size() == 0 && freeBlocks.size() == 0) {
      throw TypeException("INVALID_STATE");
    }

    // Проверяем, что множество свободных блоков freeBlocks и подмножество
    // свободных блоков в массиве blocks совпадают.
    std::set<MemoryBlock> set1(freeBlocks.begin(), freeBlocks.end()), set2;

    for (const auto &block : blocks) {
      if (block.pid() == -1) {
        set2.insert(block);
      }
    }

    if (set1 != set2) {
      throw TypeException("INVALID_STATE");
    }

    // Проверяем, что блоки памяти полностью покрывают адресное пространство.
    // Первый блок памяти должен начинаться с адреса 0.
    if (blocks.at(0).address() != 0) {
      throw TypeException("INVALID_STATE");
    }

    // Каждый блок, кроме последнего, должен заканчиваться там, где начинается
    // следующий.
    for (size_t i = 0; i < blocks.size() - 1; ++i) {
      auto &cur = blocks.at(i);
      auto &next = blocks.at(i + 1);
      if (cur.address() + cur.size() != next.address()) {
        throw TypeException("INVALID_STATE");
      }
    }

    // Последний блок должен полностью покрыть оставшееся пространство, при этом
    // не выходя за его границы.
    auto &last = blocks.back();
    if (last.address() + last.size() > 256) {
      throw TypeException("INVALID_STATE");
    }
  }
};
} // namespace MemoryManagement
