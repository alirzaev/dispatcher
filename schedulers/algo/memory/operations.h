#pragma once

#include <algorithm>
#include <cstdint>
#include <exception>
#include <tuple>
#include <vector>

#include "exceptions.h"
#include "types.h"

namespace MemoryManagement {
/**
 *  @brief Операция выделения памяти процессу в заданном блоке памяти.
 *
 *  @param state Дескриптор состояния памяти.
 *  @param blockIndex Индекс блока, в котором необходимо выделить память.
 *  @param pid Идентификатор процесса.
 *  @param pages Размер выделяемой памяти в страницах.
 *
 *  @return Новое состояние памяти.
 *
 *  @throws MemoryManagement::OperationException Исключение возникает в
 *  следующих случаях:
 *
 *  "BLOCK_IS_USED" - данный блок памяти уже занят;
 *  "TOO_SMALL" - в данном блоке невозможно выделить память: блок слишком мал.
 *
 *  @throws std::out_of_range Исключение возникает при передаче некорректного
 *  @a blockIndex.
 *
 *  Выделенная память размещается в начале блока памяти. Новый
 *  блок свободной памяти добавляется в конец списка свободных
 *  блоков памяти.
 */
inline MemoryState allocateMemory(const MemoryState &state,
                                  uint32_t blockIndex,
                                  int32_t pid,
                                  int32_t pages) {
  auto [blocks, freeBlocks] = state;

  auto block = blocks.at(blockIndex);
  if (block.pid() != -1) {
    throw OperationException("BLOCK_IS_USED");
  } else if (block.size() < pages) {
    throw OperationException("TOO_SMALL");
  }

  auto allocatedBlock = MemoryBlock(pid, block.address(), pages);
  auto freeBlockSize = block.size() - pages;
  auto freeBlockAddress = block.address() + pages;

  blocks.erase(blocks.begin() + blockIndex);
  if (freeBlockSize > 0) {
    blocks.insert(blocks.begin() + blockIndex,
                  MemoryBlock(-1, freeBlockAddress, freeBlockSize));
  }
  blocks.insert(blocks.begin() + blockIndex, allocatedBlock);

  auto pos = std::find(freeBlocks.begin(), freeBlocks.end(), block);
  freeBlocks.erase(pos);
  if (freeBlockSize > 0) {
    freeBlocks.emplace_back(-1, freeBlockAddress, freeBlockSize);
  }

  return {blocks, freeBlocks};
}

/**
 *  @brief Операция освобождения блока памяти, принадлежащего процессу.
 *
 *  @param state Дескриптор состояния памяти.
 *  @param pid Идентификатор процесса.
 *  @param blockIndex Индекс блока памяти, который необходимо освободить.
 *
 *  @return Новое состояние памяти.
 *
 *  @throws MemoryManagement::OperationException Исключение возникает в
 *  следующих случаях:
 *
 *  "PID_MISMATCH" - PID процесса, которому принадлежит блок памяти, и @a pid не
 *  совпадают.
 *
 *  @throws std::out_of_range Исключение возникает при передаче некорректного
 *  @a blockIndex.
 *
 *  Особожденный блок памяти помещается в конец списка свободных
 *  блоков памяти.
 */
inline MemoryState
freeMemory(const MemoryState &state, int32_t pid, uint32_t blockIndex) {
  auto [blocks, freeBlocks] = state;

  auto block = blocks.at(blockIndex);
  if (block.pid() != pid) {
    throw OperationException("PID_MISMATCH");
  }

  blocks[blockIndex] = MemoryBlock(-1, block.address(), block.size());
  freeBlocks.push_back(blocks[blockIndex]);

  return {blocks, freeBlocks};
}

/**
 *  @brief Операция дефрагментации памяти.
 *
 *  @param state Дескриптор состояния памяти.
 *
 *  @return Новое состояние памяти.
 *
 *  Занятые блоки памяти перемещаются в начало адресного пространства.
 *  Освободившаяся память собирается в один блок.
 */
inline MemoryState defragmentMemory(const MemoryState &state) {
  auto [blocks, freeBlocks] = state;

  int32_t address = 0;
  int32_t freeMemory = 0;
  std::vector<MemoryBlock> newBlocks;

  for (const auto &block : blocks) {
    if (block.pid() != -1) {
      newBlocks.emplace_back(block.pid(), address, block.size());
      address += block.size();
    } else {
      freeMemory += block.size();
    }
  }

  newBlocks.emplace_back(-1, address, freeMemory);
  freeBlocks = {{-1, address, freeMemory}};

  return {newBlocks, freeBlocks};
}

/**
 *  @brief Операция сжатия памяти.
 *
 *  @param state Дескриптор состояния памяти.
 *  @param startBlockIndex Первый блок памяти для сжатия.
 *
 *  @return Новое состояние памяти.
 *
 *  @throws MemoryManagement::OperationException Исключение возникает в
 *  следующих случаях:
 *
 *  "SINGLE_BLOCK" - доступен только один свободный блок, начиная с @a
 *  startBlockIndex, нечего сжимать.
 *
 *  Последовательно размещенные свободные блоки памяти сжимаются в один.
 *  Сжимается толко одна последовательность блоков, начиная с @a
 *  startBlockIndex. Новый блок свободной памяти добавляется в конец списка
 *  свободных блоков памяти.
 */
inline MemoryState compressMemory(const MemoryState &state,
                                  uint32_t startBlockIndex) {
  auto [blocks, freeBlocks] = state;

  std::vector<MemoryBlock> newBlocks(blocks.begin(),
                                     blocks.begin() + startBlockIndex);

  uint32_t currentBlock = startBlockIndex;
  uint32_t compressingBlocks = 0;
  int32_t address = blocks[startBlockIndex].address();
  int32_t freeMemory = 0;
  while (currentBlock < blocks.size() && blocks[currentBlock].pid() == -1) {
    freeMemory += blocks[currentBlock].size();
    auto pos =
        std::find(freeBlocks.begin(), freeBlocks.end(), blocks[currentBlock]);
    freeBlocks.erase(pos);
    currentBlock += 1;
    compressingBlocks += 1;
  }

  if (compressingBlocks < 2) {
    throw OperationException("SINGLE_BLOCK");
  }

  newBlocks.emplace_back(-1, address, freeMemory);
  newBlocks.insert(
      newBlocks.end(), blocks.begin() + currentBlock, blocks.end());
  freeBlocks.emplace_back(-1, address, freeMemory);

  return {newBlocks, freeBlocks};
}
} // namespace MemoryManagement
