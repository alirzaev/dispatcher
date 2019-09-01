#pragma once

#include <mapbox/variant.hpp>

#include <algo/memory/types.h>
#include <algo/processes/types.h>
#include <utils/tasks.h>

namespace Models {
struct ProcessesModel {
  ProcessesManagement::ProcessesState state;

  Utils::ProcessesTask task;
};

struct MemoryModel {
  MemoryManagement::MemoryState state;

  Utils::MemoryTask task;
};

using TaskModel = mapbox::util::variant<ProcessesModel, MemoryModel>;
} // namespace Models
