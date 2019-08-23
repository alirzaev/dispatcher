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

class UnixTaskGenerator : public AbstractTaskGenerator {
public:
  UnixTaskGenerator() = default;

  bool preemptive() const override { return true; }

  ~UnixTaskGenerator() override = default;

  optional<Request> CreateProcessReq(const ProcessesState &state,
                                     bool valid = true) const override {
    auto base = AbstractTaskGenerator::CreateProcessReq(state, valid);
    if (base) {
      auto request = get<ProcessesManagement::CreateProcessReq>(*base);

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
