#include <iomanip>
#include <iostream>

#include <nlohmann/json.hpp>

#include "generators/memory_task.h"
#include "generators/processes_task.h"

using namespace std;

int main() {
  cout << setw(4) << Generators::MemoryTask::generate(40).dump() << setw(4)
       << Generators::ProcessesTask::generate(40).dump();
  return 0;
}
