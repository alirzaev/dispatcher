#include "types.h"

namespace MemoryManagement::Types {
    MemoryState INITIAL_MEMORY_STATE{
        {MemoryBlock{-1, 0, 256}},
        {MemoryBlock{-1, 0, 256}}
    };
}
