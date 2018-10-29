#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <vector>
#include <cstdint>
#include <tuple>
#include <exception>
#include <algorithm>

#include "types.h"
#include "exceptions.h"

using namespace MemoryManagement::Types;
using namespace MemoryManagement::Exceptions;

namespace MemoryManagement::Operations {
    /*
     * Операция выделения памяти процессу в заданном блоке памяти.
     * Выделенная память размещается в начале блока памяти. Новый
     * блок свободной памяти добавляется в конец списка свободных
     * блоков памяти.
     *
     * @param state состояние памяти (список всех занятых и свободных блоков)
     * @param blockIndex индекс блока, в котором необходимо выделить память
     * @param pid идентификатор процесса
     * @param pages размер выделяемой памяти в параграфах
     *
     * @return новое состояние памяти
     */
    inline MemoryState allocateMemory(
            const MemoryState& state,
            uint32_t blockIndex,
            int32_t pid,
            int32_t pages
    )
    {
        //std::vector<MemoryBlock> blocks, freeBlocks;
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
            blocks.insert(blocks.begin() + blockIndex, MemoryBlock(-1, freeBlockAddress, freeBlockSize));
        }
        blocks.insert(blocks.begin() + blockIndex, allocatedBlock);

        auto pos = std::find(freeBlocks.begin(), freeBlocks.end(), block);
        freeBlocks.erase(pos);
        if (freeBlockSize > 0) {
            freeBlocks.push_back(MemoryBlock(-1, freeBlockAddress, freeBlockSize));
        }

        return MemoryState(blocks, freeBlocks);
    }

    /*
     * Операция освобождения блока памяти, принадлежащего процессу.
     * Особожденный блок памяти помещается в конец списка свободных
     * блоков памяти.
     *
     * @param state состояние памяти (список всех занятых и свободных блоков)
     * @param pid идентификатор процесса
     * @param blockIndex индекс блока памяти, который необходимо освободить
     *
     * @return новое состояние памяти
     */
    inline MemoryState freeMemory(
            const MemoryState& state,
            int32_t pid,
            uint32_t blockIndex
    )
    {
        //std::vector<MemoryBlock> blocks, freeBlocks;
        auto [blocks, freeBlocks] = state;

        auto block = blocks.at(blockIndex);
        if (block.pid() != pid) {
            throw OperationException("PID_MISMATCH");
        }

        blocks[blockIndex] = MemoryBlock(-1, block.address(), block.size());
        freeBlocks.push_back(blocks[blockIndex]);

		return { blocks, freeBlocks };
    }

    /*
     * Операция дефрагментации памяти. Занятые блоки памяти перемещаются
     * в начало адресного пространства. Освободившаяся память собирается
     * в один блок.
     *
     * @param state состояние памяти (список всех занятых и свободных блоков)
     *
     * @return новое состояние памяти
     */
    inline MemoryState defragmentMemory(
            const MemoryState& state
    )
    {
        //std::vector<MemoryBlock> blocks, freeBlocks;
        auto [blocks, freeBlocks] = state;

        int32_t address = 0;
        int32_t freeMemory = 0;
        std::vector<MemoryBlock> newBlocks;

        for (const auto& block : blocks) {
            if (block.pid() != -1) {
                newBlocks.emplace_back(block.pid(), address, block.size());
                address += block.size();
            } else {
                freeMemory += block.size();
            }
        }

        newBlocks.emplace_back(-1, address, freeMemory);
        freeBlocks = { MemoryBlock(-1, address, freeMemory) };

		return { newBlocks, freeBlocks };
    }

    /*
     * Сжатие памяти. Последовательно размещенные свободные блоки
     * памяти сжимаются в один. Сжимается толко одно последовательность
     * блоков, начиная с startBlockIndex. Новый блок свободной памяти
     * добавляется в конец списка свободных блоков памяти.
     *
     * @param state состояние памяти (список всех блоков и свободных)
     * @param startBlockIndex первый блок памяти для сжатия
     *
     * @return новое состояние памяти
     */
    inline MemoryState compressMemory(
            const MemoryState& state,
            uint32_t startBlockIndex
    )
    {
        //std::vector<MemoryBlock> blocks, freeBlocks;
        auto [blocks, freeBlocks] = state;

        std::vector<MemoryBlock> newBlocks(blocks.begin(), blocks.begin() + startBlockIndex);

        uint32_t currentBlock = startBlockIndex;
		uint32_t compressingBlocks = 0;
        int32_t address = blocks[startBlockIndex].address();
        int32_t freeMemory = 0;
        while (currentBlock < blocks.size() && blocks[currentBlock].pid() == -1) {
            freeMemory += blocks[currentBlock].size();
            auto pos = std::find(freeBlocks.begin(), freeBlocks.end(), blocks[currentBlock]);
            freeBlocks.erase(pos);
            currentBlock += 1;
			compressingBlocks += 1;
        }

		if (compressingBlocks < 2) {
			throw OperationException("SINGLE_BLOCK");
		}

        newBlocks.emplace_back(-1, address, freeMemory);
        newBlocks.insert(newBlocks.end(), blocks.begin() + currentBlock, blocks.end());
        freeBlocks.emplace_back(-1, address, freeMemory);

		return { newBlocks, freeBlocks };
    }
}

#endif // OPERATIONS_H
