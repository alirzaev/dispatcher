#pragma once

#include "task_abstract_generator.h"

namespace Generators::ProcessesTask::TaskGenerators {

class RoundRobinTaskGenerator : public AbstractTaskGenerator {
public:
  RoundRobinTaskGenerator() = default;

  bool preemptive() const override { return true; }

  ~RoundRobinTaskGenerator() override = default;
};
} // namespace Generators::ProcessesTask::TaskGenerators
