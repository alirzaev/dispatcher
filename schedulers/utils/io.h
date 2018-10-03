#ifndef IO_H
#define IO_H

#include <vector>
#include <istream>
#include <ostream>

#include "../algo/memory/requests.h"
#include "../algo/memory/types.h"
#include "../algo/memory/strategies.h"

#include "tasks.h"

namespace Utils {
namespace IO {
    std::vector<Tasks::TaskPtr> loadTasks(std::istream& is);

    void saveTasks(const std::vector<Tasks::TaskPtr>& tasks, std::ostream& os)
    {
        auto obj = nlohmann::json::array();

        for (auto task : tasks) {
            obj.push_back(task->dump());
        }

        os << obj;
    }
}
}

#endif // IO_H
