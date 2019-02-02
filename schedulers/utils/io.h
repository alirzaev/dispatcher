#pragma once

#include <istream>
#include <ostream>
#include <variant>
#include <vector>

#include "tasks.h"

namespace Utils::details {
using Utils::TaskException;

inline MemoryTask loadMemoryTask(const nlohmann::json &obj) {
  using namespace MemoryManagement;
  StrategyPtr strategy;

  auto strategyType = obj["strategy"];
  if (strategyType == "FIRST_APPROPRIATE") {
    strategy = FirstAppropriateStrategy::create();
  } else if (strategyType == "MOST_APPROPRIATE") {
    strategy = MostAppropriateStrategy::create();
  } else if (strategyType == "LEAST_APPROPRIATE") {
    strategy = LeastAppropriateStrategy::create();
  } else {
    throw TaskException("UNKNOWN_STRATEGY");
  }

  uint32_t completed = obj["completed"];

  std::vector<Request> requests;
  for (auto req : obj["requests"]) {
    if (req["type"] == "CREATE_PROCESS") {
      requests.push_back(CreateProcessReq(req["pid"], req["bytes"], req["pages"]));
    } else if (req["type"] == "TERMINATE_PROCESS") {
      requests.push_back(TerminateProcessReq(req["pid"]));
    } else if (req["type"] == "ALLOCATE_MEMORY") {
      requests.push_back(
          AllocateMemory(req["pid"], req["bytes"], req["pages"]));
    } else if (req["type"] == "FREE_MEMORY") {
      requests.push_back(FreeMemory(req["pid"], req["address"]));
    } else {
      throw TaskException("UNKNOWN_REQUEST");
    }
  }

  std::vector<MemoryBlock> blocks, freeBlocks;
  for (auto block : obj["state"]["blocks"]) {
    blocks.emplace_back(block["pid"], block["address"], block["size"]);
  }
  for (auto blockObj : obj["state"]["free_blocks"]) {
    freeBlocks.emplace_back(blockObj["pid"], blockObj["address"],
                            blockObj["size"]);
  }

  return MemoryTask::create(strategy, completed, {blocks, freeBlocks},
                            requests);
}
} // namespace Utils::details

namespace Utils {
inline std::vector<Task> loadTasks(std::istream &is) {
  nlohmann::json obj;
  is >> obj;
  std::vector<Task> tasks;

  for (auto task : obj) {
    if (task["type"] == "MEMORY_TASK") {
      tasks.emplace_back(details::loadMemoryTask(task));
    } else {
      throw TaskException("UNKNOWN_TASK");
    }
  }

  return tasks;
}

inline void saveTasks(const std::vector<Task> &tasks, std::ostream &os) {
  auto obj = nlohmann::json::array();

  for (Task task : tasks) {
    std::visit([&obj](const auto &task) { obj.push_back(task.dump()); }, task);
  }

  os << obj;
}
} // namespace Utils
