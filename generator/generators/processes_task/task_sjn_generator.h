#pragma once

#include <mapbox/variant.hpp>
#include <tl/optional.hpp>

#include <algo/processes/requests.h>
#include <algo/processes/types.h>

#include "../rand_utils.h"

#include "task_abstract_generator.h"

namespace Generators::ProcessesTask::TaskGenerators {
using namespace ProcessesManagement;
using tl::nullopt;
using tl::optional;

class SjnTaskGenerator : public AbstractTaskGenerator {
public:
  SjnTaskGenerator() = default;

  bool preemptive() const override { return false; }

  ~SjnTaskGenerator() override = default;

  optional<Request> CreateProcessReq(const ProcessesState &state,
                                     bool valid = true) const override {
    auto base = AbstractTaskGenerator::CreateProcessReq(state, valid);
    if (base) {
      auto request = base->get<ProcessesManagement::CreateProcessReq>();

      auto workTime = RandUtils::randRange(4, 32);
      return ProcessesManagement::CreateProcessReq(request.pid(),
                                                   request.ppid(),
                                                   request.priority(),
                                                   request.basePriority(),
                                                   request.timer(),
                                                   workTime);
    } else {
      return base;
    }
  }
};
} // namespace Generators::ProcessesTask::TaskGenerators
