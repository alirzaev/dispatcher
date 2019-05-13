#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

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
        {WinNtStrategy::create(), make_shared<WinNtTaskGenerator>()}};
  } else {
    strategies = {{FcfsStrategy::create(), make_shared<FcfsTaskGenerator>()},
                  {SjnStrategy::create(), make_shared<SjnTaskGenerator>()},
                  {SrtStrategy::create(), make_shared<SrtTaskGenerator>()}};
  }

  return RandUtils::randChoice(strategies.begin(), strategies.end());
}
} // namespace Generators::ProcessesTask::Details

namespace Generators::ProcessesTask {
inline Utils::ProcessesTask generate(uint32_t requestCount = 40,
                                     bool preemptive = false) {
  using namespace Details;
  using namespace ProcessesManagement;
  using namespace RandUtils;
  using std::nullopt;
  using std::optional;
  using std::vector;

  auto [strategy, generator] = randStrategy(preemptive);
  auto state = ProcessesState::initial();
  vector<Request> requests;

  for (uint32_t i = 0; i < requestCount; ++i) {
    bool validRequired = i == 0 ? true : randRange(0, 256) % 8 > 0;
    optional<Request> last =
        requests.empty() ? nullopt : optional(requests.back());
    vector<Request> validRequests = generator->generate(state, last, true),
                    invalidRequests = generator->generate(state, last, false);

    if (validRequired && !validRequests.empty()) {
      requests.push_back(
          randChoice(validRequests.begin(), validRequests.end()));
    } else if (!validRequired && !invalidRequests.empty()) {
      requests.push_back(
          randChoice(invalidRequests.begin(), invalidRequests.end()));
    } else if (validRequired && validRequests.empty()) {
      requests.push_back(
          randChoice(invalidRequests.begin(), invalidRequests.end()));
    } else {
      requests.push_back(
          randChoice(validRequests.begin(), validRequests.end()));
    }

    state = strategy->processRequest(requests.back(), state);
  }
  return Utils::ProcessesTask::create(strategy, 0, ProcessesState::initial(),
                                      requests);
}
} // namespace Generators::ProcessesTask
