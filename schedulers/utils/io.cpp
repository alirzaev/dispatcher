#include <memory>
#include <tuple>
#include <vector>

#include "../../libs/nlohmann/json.hpp"

#include "io.h"
#include "tasks.h"
#include "../algo/memory/strategies.h"
#include "../algo/memory/types.h"
#include "../algo/memory/requests.h"

using namespace MemoryManagement::Strategies;
using namespace MemoryManagement::Types;
using namespace MemoryManagement;

nlohmann::json dump(const MemoryBlock& block) {
    return nlohmann::json{
        {"pid", block.pid()},
        {"address", block.address()},
        {"size", block.size()}
    };
}

nlohmann::json dumpMemoryTask(Utils::Tasks::MemoryTask* task)
{
    nlohmann::json obj;

    auto strategy = task->strategy();
    if (strategy->type == StrategyType::FIRST_APPROPRIATE) {
        obj["strategy"] = "FIRST_APPROPRIATE";
    } else if (strategy->type == StrategyType::MOST_APPROPRIATE) {
        obj["strategy"] = "MOST_APPROPRIATE";
    } else {
        obj["strategy"] = "LEAST_APPROPRIATE";
    }

    obj["completed"] = task->completed();

    std::vector<MemoryBlock> blocks, freeBlocks;
    std::tie(blocks, freeBlocks) = task->state();
    auto jsonBlocks = nlohmann::json::array();
    auto jsonFreeBlocks = nlohmann::json::array();

    for (const auto& block : blocks) {
        jsonBlocks.push_back(dump(block));
    }
    for (const auto& block : freeBlocks) {
        jsonFreeBlocks.push_back(dump(block));
    }
    obj["state"] = {
        {"blocks", jsonBlocks},
        {"free_blocks", jsonFreeBlocks}
    };

    auto jsonRequests = nlohmann::json::array();

    for (auto request : task->requests()) {
        jsonRequests.push_back(request->dump());
    }

    obj["requests"] = jsonRequests;

    return obj;
}

namespace Utils {
namespace Tasks {
    std::vector<Tasks::TaskPtr> loadTasks(std::istream& is);

    void saveTasks(const std::vector<Tasks::TaskPtr>& tasks, std::ostream& os);
}
}
