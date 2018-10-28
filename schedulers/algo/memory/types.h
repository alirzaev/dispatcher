#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <cstdint>
#include <tuple>
#include <nlohmann/json.hpp>

namespace MemoryManagement {
    namespace Types {
    class MemoryBlock {
    private:
        int32_t _pid;

        int32_t _address;

        int32_t _size;

    public:
        int32_t pid() const
        {
            return _pid;
        }

        int32_t address() const
        {
            return _address;
        }

        int32_t size() const
        {
            return _size;
        }

        MemoryBlock(int32_t pid, int32_t address, int32_t size) :
            _pid(pid), _address(address), _size(size)
        {}

        MemoryBlock(const MemoryBlock& other) = default;

        MemoryBlock() :
            _pid(-1), _address(0), _size(0)
        {}

        MemoryBlock& operator=(const MemoryBlock& rhs) = default;

        bool operator==(const MemoryBlock& rhs) const {
            return _pid == rhs._pid &&
                    _address == rhs._address &&
                    _size == rhs._size;
        }

        nlohmann::json dump() const
        {
            return {
                {"pid", _pid},
                {"address", _address},
                {"size", _size}
            };
        }
    };

    class MemoryState
    {
    public:
        std::vector<MemoryBlock> blocks;

        std::vector<MemoryBlock> freeBlocks;

        MemoryState(const std::vector<MemoryBlock>& blocks,
                    const std::vector<MemoryBlock>& freeBlocks) :
            blocks(blocks), freeBlocks(freeBlocks)
        {}

        MemoryState() = default;

        MemoryState(const MemoryState& state) = default;

        MemoryState(MemoryState&& state) = default;

        MemoryState& operator=(const MemoryState& state) = default;

        MemoryState& operator=(MemoryState&& state) = default;

        bool operator==(const MemoryState& state) const
        {
            return blocks == state.blocks && freeBlocks == state.freeBlocks;
        }

        bool operator!=(const MemoryState& state) const
        {
            return !(*this == state);
        }

        nlohmann::json dump() const
        {
            auto jsonBlocks = nlohmann::json::array();
            auto jsonFreeBlocks = nlohmann::json::array();

            for (const auto& block : blocks) {
                jsonBlocks.push_back(block.dump());
            }
            for (const auto& block : freeBlocks) {
                jsonFreeBlocks.push_back(block.dump());
            }

            return {
                {"blocks", jsonBlocks},
                {"free_blocks", jsonFreeBlocks}
            };
        }

        static MemoryState initial()
        {
            return {
                {MemoryBlock{-1, 0, 256}},
                {MemoryBlock{-1, 0, 256}}
            };
        }
    };
    }
}
#endif // TYPES_H
