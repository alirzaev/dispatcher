#pragma once

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <tl/optional.hpp>

#include <algo/processes/requests.h>
#include <algo/processes/strategies.h>
#include <algo/processes/types.h>
#include <utils/tasks.h>

#include "processes_task/task_generators.h"
#include "rand_utils.h"

namespace Generators::ProcessesTask::Details {
using namespace ProcessesManagement;
using namespace TaskGenerators;
using std::make_shared;
using std::pair;
using std::shared_ptr;
using std::vector;

using GeneratorPtr = shared_ptr<AbstractTaskGenerator>;

inline pair<StrategyPtr, GeneratorPtr> randStrategy(bool preemptive = false) {
  vector<pair<StrategyPtr, GeneratorPtr>> strategies;

  if (preemptive) {
    strategies = {
        {RoundRobinStrategy::create(), make_shared<RoundRobinTaskGenerator>()},
        {WinNtStrategy::create(), make_shared<WinNtTaskGenerator>()},
        {UnixStrategy::create(), make_shared<UnixTaskGenerator>()}};
  } else {
    strategies = {{FcfsStrategy::create(), make_shared<FcfsTaskGenerator>()},
                  {SjnStrategy::create(), make_shared<SjnTaskGenerator>()},
                  {SrtStrategy::create(), make_shared<SrtTaskGenerator>()}};
  }

  return RandUtils::randChoice(strategies);
}
} // namespace Generators::ProcessesTask::Details

namespace Generators::ProcessesTask {
inline Utils::ProcessesTask generate(uint32_t requestCount = 40,
                                     bool preemptive = false) {
  using namespace Details;
  using namespace ProcessesManagement;
  using namespace RandUtils;
  using std::vector;
  using tl::nullopt;
  using tl::optional;

  auto [strategy, generator] = randStrategy(preemptive);
  auto state = ProcessesState::initial();
  vector<Request> requests;
  bool isLastValid = true;

  for (uint32_t i = 0; i < requestCount; ++i) {
    bool validRequired = i == 0 ? true : randRange(0, 256) % 16 > 0;
    optional<Request> last =
        requests.empty() ? nullopt : optional(requests.back());
    vector<Request> validRequests =
                        generator->generate(state, {last, isLastValid}, true),
                    invalidRequests =
                        generator->generate(state, {last, isLastValid}, false);

    isLastValid = (validRequired && !validRequests.empty()) ||
                  (!validRequired && invalidRequests.empty());

    if (isLastValid) {
      requests.push_back(randChoice(validRequests));
    } else {
      requests.push_back(randChoice(invalidRequests));
    }

    state = strategy->processRequest(requests.back(), state);
  }
  return Utils::ProcessesTask::create(
      strategy, 0, ProcessesState::initial(), requests);
}
} // namespace Generators::ProcessesTask
