#ifndef IO_H
#define IO_H

#include <vector>
#include <istream>
#include <ostream>

#include "tasks.h"

namespace Utils::IO {
    std::vector<Tasks::Task> loadTasks(std::istream& is);

    void saveTasks(const std::vector<Tasks::Task>& tasks, std::ostream& os);
}

#endif // IO_H
