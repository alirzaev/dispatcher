#include "io.h"

using namespace MemoryManagement::Strategies;
using namespace MemoryManagement::Types;
using namespace MemoryManagement::Requests;

Utils::Tasks::MemoryTask loadMemoryTask(const nlohmann::json& obj) {
    StrategyPtr strategy;

    auto strategyType = obj["strategy"];
    if (strategyType == "FIRST_APPROPRIATE") {
        strategy = FirstAppropriateStrategy::create();
    }
    else if (strategyType == "MOST_APPROPRIATE") {
        strategy = MostAppropriateStrategy::create();
    }
    else if (strategyType == "LEAST_APPROPRIATE") {
        strategy = LeastAppropriateStrategy::create();
    }
    else {
        throw Utils::Exceptions::TaskException("UNKNOWN_STRATEGY");
    }

    uint32_t completed = obj["completed"];

    std::vector<RequestPtr> requests;
    for (auto reqObj : obj["requests"]) {
        if (reqObj["type"] == "CREATE_PROCESS") {
            requests.push_back(CreateProcess::create(
                reqObj["pid"], reqObj["bytes"], reqObj["pages"]
            ));
        }
        else if (reqObj["type"] == "TERMINATE_PROCESS") {
            requests.push_back(TerminateProcess::create(
                reqObj["pid"]
            ));
        }
        else if (reqObj["type"] == "ALLOCATE_MEMORY") {
            requests.push_back(AllocateMemory::create(
                reqObj["pid"], reqObj["bytes"], reqObj["pages"]
            ));
        }
        else if (reqObj["type"] == "FREE_MEMORY") {
            requests.push_back(FreeMemory::create(
                reqObj["pid"], reqObj["address"]
            ));
        }
        else {
            throw Utils::Exceptions::TaskException("UNKNOWN_REQUEST");
        }
    }

    std::vector<MemoryBlock> blocks, freeBlocks;
    for (auto blockObj : obj["state"]["blocks"]) {
        blocks.emplace_back(blockObj["pid"], blockObj["address"], blockObj["size"]);
    }
    for (auto blockObj : obj["state"]["free_blocks"]) {
        freeBlocks.emplace_back(blockObj["pid"], blockObj["address"], blockObj["size"]);
    }

    return Utils::Tasks::MemoryTask::create(strategy, completed, MemoryState{ blocks, freeBlocks }, requests);
}

namespace Utils::IO {
    std::vector<Tasks::Task> loadTasks(std::istream& is)
    {
        nlohmann::json obj;
        is >> obj;
        std::vector<Tasks::Task> tasks;

        for (auto taskObj : obj) {
            if (taskObj["type"] == "MEMORY_TASK") {
                tasks.push_back(loadMemoryTask(taskObj));
            }
            else {
                throw Utils::Exceptions::TaskException("UNKNOWN_TASK");
            }
        }

        return tasks;
    }

    void saveTasks(const std::vector<Tasks::Task>& tasks, std::ostream& os)
    {
        auto obj = nlohmann::json::array();

        for (auto task : tasks) {
            if (auto* p = std::get_if<Tasks::MemoryTask>(&task)) {
                obj.push_back(p->dump());
            }
        }

        os << obj;
    }
}
