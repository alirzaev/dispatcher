#ifndef MODELS_H
#define MODELS_H

#include <variant>

#include "../schedulers/utils/tasks.h"
#include "../schedulers/algo/memory/types.h"

namespace Models
{
struct ProcessesModel
{

};

struct MemoryModel
{
    MemoryManagement::Types::MemoryState state;

    Utils::Tasks::MemoryTask task;
};

using TaskModel = std::variant<ProcessesModel, MemoryModel>;
}

#endif // MODELS_H
