#pragma once

#include "task_roundrobin_generator.h"

namespace Generators::ProcessesTask::TaskGenerators {
class FcfsTaskGenerator : public RoundRobinTaskGenerator {
  bool preemptive() const override { return false; }
};
} // namespace Generators::ProcessesTask::TaskGenerators
