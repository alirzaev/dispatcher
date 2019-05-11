#pragma once

#include <optional>

#include <algo/processes/requests.h>
#include <algo/processes/types.h>

#include "../rand_utils.h"

#include "task_abstract_generator.h"

namespace Generators::ProcessesTask::TaskGenerators {
using namespace ProcessesManagement;
using std::get;
using std::nullopt;
using std::optional;

class WinNtTaskGenerator : public AbstractTaskGenerator {
public:
  WinNtTaskGenerator() = default;

  bool preemptive() const override { return true; }

  ~WinNtTaskGenerator() override = default;

  optional<Request> CreateProcessReq(const ProcessesState &state,
                                     bool valid = true) const override {
    auto base = AbstractTaskGenerator::CreateProcessReq(state, valid);
    if (base) {
      auto request = get<ProcessesManagement::CreateProcessReq>(*base);

      auto priority = RandUtils::randRange(0, 11);
      return ProcessesManagement::CreateProcessReq(
          request.pid(), request.ppid(), priority, priority, request.timer(),
          request.workTime());
    } else {
      return base;
    }
  }

  optional<Request> TerminateIO(const ProcessesState &state,
                                bool valid = true) const override {
    auto base = AbstractTaskGenerator::TerminateIO(state, valid);
    if (base) {
      auto request = get<ProcessesManagement::TerminateIO>(*base);

      auto pid = request.pid();
      auto augment = RandUtils::randRange(1, 3);
      return ProcessesManagement::TerminateIO(pid, augment);
    } else {
      return base;
    }
  }
};
} // namespace Generators::ProcessesTask::TaskGenerators
