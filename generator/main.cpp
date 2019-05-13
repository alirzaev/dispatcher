#include <iomanip>
#include <iostream>

#include <nlohmann/json.hpp>

#include "generators/memory_task.h"
#include "generators/processes_task.h"

using namespace std;

int main() {
  nlohmann::json tasks = nlohmann::json::array();

  tasks.push_back(Generators::MemoryTask::generate(40).dump());
  tasks.push_back(Generators::ProcessesTask::generate(40, false).dump());
  tasks.push_back(Generators::ProcessesTask::generate(40, true).dump());

  cout << setw(4) << tasks;
  return 0;
}
