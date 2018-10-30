#pragma once

#include <variant>

#include <algo/memory/types.h>
#include <utils/tasks.h>

namespace Models {
struct ProcessesModel {};

struct MemoryModel {
  MemoryManagement::Types::MemoryState state;

  Utils::Tasks::MemoryTask task;
};

using TaskModel = std::variant<ProcessesModel, MemoryModel>;
} // namespace Models
