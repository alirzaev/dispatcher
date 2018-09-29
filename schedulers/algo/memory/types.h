#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <cstdint>
#include <tuple>

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
    };

    using MemoryState = std::tuple<
        std::vector<MemoryBlock>,
        std::vector<MemoryBlock>
    >;
    }
}
#endif // TYPES_H
