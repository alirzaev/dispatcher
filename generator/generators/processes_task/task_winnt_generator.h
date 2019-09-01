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

class WinNtTaskGenerator : public AbstractTaskGenerator {
public:
  WinNtTaskGenerator() = default;

  bool preemptive() const override { return true; }

  ~WinNtTaskGenerator() override = default;

  optional<Request> CreateProcessReq(const ProcessesState &state,
                                     bool valid = true) const override {
    auto base = AbstractTaskGenerator::CreateProcessReq(state, valid);
    if (base) {
      auto request = base->get<ProcessesManagement::CreateProcessReq>();

      auto priority = RandUtils::randRange(0u, 11u);
      return ProcessesManagement::CreateProcessReq(request.pid(),
                                                   request.ppid(),
                                                   priority,
                                                   priority,
                                                   request.timer(),
                                                   request.workTime());
    } else {
      return base;
    }
  }

  optional<Request> TerminateIO(const ProcessesState &state,
                                bool valid = true) const override {
    auto base = AbstractTaskGenerator::TerminateIO(state, valid);
    if (base) {
      auto request = base->get<ProcessesManagement::TerminateIO>();

      auto pid = request.pid();
      auto augment = RandUtils::randRange(1u, 3u);
      return ProcessesManagement::TerminateIO(pid, augment);
    } else {
      return base;
    }
  }
};
} // namespace Generators::ProcessesTask::TaskGenerators
