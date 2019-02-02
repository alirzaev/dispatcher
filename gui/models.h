#pragma once

#include <variant>

#include <algo/memory/types.h>
#include <utils/tasks.h>

namespace Models {
struct ProcessesModel {};

struct MemoryModel {
  MemoryManagement::MemoryState state;

  Utils::MemoryTask task;
};

using TaskModel = std::variant<ProcessesModel, MemoryModel>;
} // namespace Models
