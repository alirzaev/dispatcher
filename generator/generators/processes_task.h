#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <set>
#include <type_traits>
#include <utility>
#include <vector>

#include <algo/processes/requests.h>
#include <algo/processes/strategies.h>
#include <algo/processes/types.h>
#include <utils/tasks.h>

#include "processes_task/task_generators.h"

namespace Generators::ProcessesTask::Details {
using namespace ProcessesManagement;
using namespace TaskGenerators;
using std::make_shared;
using std::shared_ptr;
using std::vector;

using GeneratorPtr = shared_ptr<AbstractTaskGenerator>;

template <class I, typename = std::enable_if_t<std::is_integral_v<I>>>
inline I randRange(I a, I b) {
  if (a > b) {
    std::swap(a, b);
  }
  static std::mt19937 e(static_cast<unsigned int>(std::time(nullptr)));
  std::uniform_int_distribution<I> dist(a, b);
  return dist(e);
}

template <class BidIt,
          typename = std::enable_if_t<!std::is_fundamental_v<BidIt>>>
inline auto randChoice(BidIt first, BidIt last) -> decltype(*first) {
  auto size = std::distance(first, last);
  auto index = randRange<decltype(size)>(0, size - 1);
  std::advance(first, index);
  return *first;
}

inline StrategyPtr randStrategy() {
  vector<StrategyPtr> strategies = {RoundRobinStrategy::create()};

  return randChoice(strategies.begin(), strategies.end());
}

inline GeneratorPtr randGenerator() {
  vector<GeneratorPtr> generators = {make_shared<RoundRobinTaskGenerator>()};

  return randChoice(generators.begin(), generators.end());
}
} // namespace Generators::ProcessesTask::Details

namespace Generators::ProcessesTask {
inline Utils::ProcessesTask generate(uint32_t requestCount = 40) {
  using namespace Details;
  using namespace ProcessesManagement;
  using std::vector;

  auto generator = randGenerator();
  auto strategy = randStrategy();
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
