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

class SjtTaskGenerator : public AbstractTaskGenerator {
public:
  SjtTaskGenerator() = default;

  bool preemptive() const override { return false; }

  ~SjtTaskGenerator() override = default;

  optional<Request> CreateProcessReq(const ProcessesState &state,
                                     bool valid = true) const override {
    auto base = AbstractTaskGenerator::CreateProcessReq(state, valid);
    if (base) {
      auto request = get<ProcessesManagement::CreateProcessReq>(*base);

      auto workTime = RandUtils::randRange(4, 32);
      return ProcessesManagement::CreateProcessReq(
          request.pid(), request.ppid(), request.priority(),
          request.basePriority(), request.timer(), workTime);
    } else {
      return base;
    }
  }
};
} // namespace Generators::ProcessesTask::TaskGenerators
