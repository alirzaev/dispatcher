#pragma once

#include "operations.h"
#include "requests.h"
#include "types.h"

#include <algorithm>
#include <cstdint>
#include <exception>
#include <memory>
#include <string>
#include <variant>

namespace MemoryManagement {
using std::shared_ptr;

enum class StrategyType {
  FIRST_APPROPRIATE,
  MOST_APPROPRIATE,
  LEAST_APPROPRIATE
};

class AbstractStrategy {
public:
  virtual ~AbstractStrategy() {}

  AbstractStrategy(StrategyType type) : type(type) {}

  const StrategyType type;

  virtual std::string toString() const = 0;

  MemoryState processRequest(const Request &request,
                             const MemoryState &state) const {
    return std::visit(
        [this, state](const auto &req) {
          return this->processRequest(req, state);
        },
        request);
  }

  MemoryState processRequest(const CreateProcessReq &request,
                             const MemoryState &state) const {
    return sortFreeBlocks(allocateMemoryGeneral(
        AllocateMemory(request.pid(), request.bytes(), request.pages()), state,
        true));
  }

  MemoryState processRequest(const TerminateProcessReq &request,
                             const MemoryState &state) const {
    auto currentState = state;

    while (true) {
      auto [blocks, freeBlocks] = currentState;
      // ищем очередной блок памяти, выделенный процессу
      auto pos = std::find_if(blocks.begin(), blocks.end(),
                              [&request](const auto &block) {
                                return request.pid() == block.pid();
                              });
      if (pos == blocks.end()) {
        break;
      }

      // освобождаем блок
      uint32_t index = static_cast<uint32_t>(pos - blocks.begin());
      currentState = freeMemory(currentState, request.pid(), index);
    }

    // сжатие памяти
    // переупорядочение свободных блоков памяти
    return sortFreeBlocks(compressAllMemory(currentState));
  }

  MemoryState processRequest(const AllocateMemory &request,
                             const MemoryState &state) const {
    return sortFreeBlocks(allocateMemoryGeneral(request, state, false));
  }

  MemoryState processRequest(const FreeMemory &request,
                             const MemoryState &state) const {
    auto currentState = state;
    auto [blocks, freeBlocks] = currentState;

    // ищем блок, начинающийся с заданного адреса
    auto pos = std::find_if(blocks.begin(), blocks.end(),
                            [&request](const auto &block) {
                              return block.address() == request.address();
                            });
    // если такого блока нет, игнорируем заявку
    if (pos == blocks.end()) {
      return state;
    }
    // если блок выделен другому процессу, игнорируем заявку
    if (pos->pid() != request.pid()) {
      return state;
    }

    // освобождаем блок
    uint32_t index = static_cast<uint32_t>(pos - blocks.begin());
    currentState = freeMemory(state, request.pid(), index);

    // сжатие памяти
    // переупорядочение свободных блоков памяти
    return sortFreeBlocks(compressAllMemory(currentState));
  }

protected:
  virtual MemoryState sortFreeBlocks(const MemoryState &state) const = 0;

  virtual std::vector<MemoryBlock>::const_iterator
  findFreeBlock(const std::vector<MemoryBlock> &blocks,
                const std::vector<MemoryBlock> &freeBlocks,
                int32_t size) const final {
    auto freeBlockPos = std::find_if(
        freeBlocks.cbegin(), freeBlocks.cend(),
        [&size](const auto &block) { return size <= block.size(); });
    if (freeBlockPos != freeBlocks.cend()) {
      return std::find(blocks.cbegin(), blocks.cend(), *freeBlockPos);
    } else {
      return blocks.cend();
    }
  }

  virtual MemoryState compressAllMemory(const MemoryState &state) const final {
    auto currentState = state;

    while (true) {
      auto [blocks, freeBlocks] = currentState;
      // ищем первый свободный блок памяти
      // проверяем, есть ли за ним хотя бы один свободный блок
      uint32_t index = 0;
      for (; index < blocks.size() - 1 &&
             !(blocks[index].pid() == -1 && blocks[index + 1].pid() == -1);
           ++index) {
      }

      // если есть, то выполняем сжатие
      if (index < blocks.size() - 1) {
        currentState = compressMemory(currentState, index);
      } else {
        break;
      }
    }

    return currentState;
  }

  virtual MemoryState
  allocateMemoryGeneral(const AllocateMemory &request, const MemoryState &state,
                        const bool createProcess = false) const final {
    auto [blocks, freeBlocks] = state;

    // проверить, выделены ли процессу какие-либо блоки памяти
    auto processPos = std::find_if(
        blocks.begin(), blocks.end(),
        [&request](const auto &block) { return block.pid() == request.pid(); });
    // обработка некорректных ситуаций:
    // 1. Создание уже существующего процесса
    // 2. Выделение памяти несуществующему процессу
    if ((processPos != blocks.end() && createProcess) ||
        (processPos == blocks.end() && !createProcess)) {
      return state;
    }

    int32_t totalFree = 0;
    for (const auto &block : freeBlocks) {
      totalFree += block.size();
    }

    // проверить, если свободный блок подходящего размера
    // если есть, то выделить процессу память в этом блоке
    if (auto pos = findFreeBlock(blocks, freeBlocks, request.pages());
        pos != blocks.end()) {
      uint32_t index = static_cast<uint32_t>(pos - blocks.cbegin());

      return allocateMemory(state, index, request.pid(), request.pages());
    } else if (totalFree >=
               request.pages()) { // если суммарно свободной памяти достаточно,
                                  // то выполнить дефрагментацию
      auto newState = defragmentMemory(state);
      auto [blocks, freeBlocks] = newState;
      auto pos = findFreeBlock(blocks, freeBlocks, request.pages());
      uint32_t index = static_cast<uint32_t>(pos - blocks.cbegin());

      return allocateMemory(newState, index, request.pid(), request.pages());
    } else { // недостаточно свободной памяти, проигнорировать заявку
      return state;
    }
  }
};

using StrategyPtr = shared_ptr<AbstractStrategy>;

class FirstAppropriateStrategy final : public AbstractStrategy {
public:
  std::string toString() const override { return "FIRST_APPROPRIATE"; }

  static shared_ptr<FirstAppropriateStrategy> create() {
    return shared_ptr<FirstAppropriateStrategy>(new FirstAppropriateStrategy());
  }

protected:
  MemoryState sortFreeBlocks(const MemoryState &state) const override {
    auto currentState = state;
    auto [blocks, freeBlocks] = currentState;

    // упорядочиваем блоки:
    // - по начальному адресу в порядке возрастания
    std::stable_sort(freeBlocks.begin(), freeBlocks.end(),
                     [](const auto &left, const auto &right) {
                       return left.address() < right.address();
                     });

    return {blocks, freeBlocks};
  }

private:
  FirstAppropriateStrategy()
      : AbstractStrategy(StrategyType::FIRST_APPROPRIATE) {}
};

class MostAppropriateStrategy final : public AbstractStrategy {
public:
  std::string toString() const override { return "MOST_APPROPRIATE"; }

  static shared_ptr<MostAppropriateStrategy> create() {
    return shared_ptr<MostAppropriateStrategy>(new MostAppropriateStrategy());
  }

private:
  MostAppropriateStrategy()
      : AbstractStrategy(StrategyType::MOST_APPROPRIATE) {}

protected:
  MemoryState sortFreeBlocks(const MemoryState &state) const override {
    auto currentState = state;
    auto [blocks, freeBlocks] = currentState;

    // упорядочиваем блоки:
    // - по размеру в порядке возрастания
    // - по начальному адресу в порядке возрастания
    std::stable_sort(freeBlocks.begin(), freeBlocks.end(),
                     [](const auto &left, const auto &right) {
                       if (left.size() == right.size()) {
                         return left.address() < right.address();
                       } else {
                         return left.size() < right.size();
                       }
                     });

    return {blocks, freeBlocks};
  }
};

class LeastAppropriateStrategy final : public AbstractStrategy {
public:
  std::string toString() const override { return "LEAST_APPROPRIATE"; }

  static shared_ptr<LeastAppropriateStrategy> create() {
    return shared_ptr<LeastAppropriateStrategy>(new LeastAppropriateStrategy());
  }

private:
  LeastAppropriateStrategy()
      : AbstractStrategy(StrategyType::LEAST_APPROPRIATE) {}

protected:
  MemoryState sortFreeBlocks(const MemoryState &state) const override {
    auto currentState = state;
    auto [blocks, freeBlocks] = currentState;

    // упорядочиваем блоки:
    // - по размеру в порядке убывания
    // - по начальному адресу в порядке возрастания
    std::stable_sort(freeBlocks.begin(), freeBlocks.end(),
                     [](const auto &left, const auto &right) {
                       if (left.size() == right.size()) {
                         return left.address() < right.address();
                       } else {
                         return left.size() > right.size();
                       }
                     });

    return {blocks, freeBlocks};
  }
};
} // namespace MemoryManagement
