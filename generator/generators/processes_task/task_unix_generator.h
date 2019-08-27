#pragma once

#include <optional>

#include <mapbox/variant.hpp>

#include <algo/processes/requests.h>
#include <tl/optional.hpp>

#include <algo/processes/types.h>

#include "../rand_utils.h"

#include "task_abstract_generator.h"

namespace Generators::ProcessesTask::TaskGenerators {
using namespace ProcessesManagement;
using tl::nullopt;
using tl::optional;

class UnixTaskGenerator : public AbstractTaskGenerator {
public:
  UnixTaskGenerator() = default;

  bool preemptive() const override { return true; }

  ~UnixTaskGenerator() override = default;

  optional<Request> CreateProcessReq(const ProcessesState &state,
                                     bool valid = true) const override {
    auto base = AbstractTaskGenerator::CreateProcessReq(state, valid);
    if (base) {
      auto request = base->get<ProcessesManagement::CreateProcessReq>();

      auto priority = RandUtils::randRange(0u, 11u);
      return ProcessesManagement::CreateProcessReq(request.pid(),
                                                   request.ppid(),
                                                   priority,
                                                   0,
                                                   request.timer(),
                                                   request.workTime());
    } else {
      return base;
    }
  }
};
} // namespace Generators::ProcessesTask::TaskGenerators
