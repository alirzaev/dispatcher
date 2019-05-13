#pragma once

#include <algorithm>
#include <cstdint>
#include <exception>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <variant>

#include "../../../utils/overload.h"
#include "../exceptions.h"
#include "../operations.h"
#include "sjn.h"

namespace ProcessesManagement {
/**
 *  @brief Стратегия "SRT".
 */
class SrtStrategy final : public SjnStrategy {
public:
  StrategyType type() const override { return StrategyType::SRT; }

  std::string toString() const override { return "SRT"; }

  static std::shared_ptr<SrtStrategy> create() {
    return std::shared_ptr<SrtStrategy>(new SrtStrategy());
  }

private:
  SrtStrategy() : SjnStrategy() {}

  ProcessesState sortQueues(const ProcessesState &state) const override {
    auto [processes, queues] = state;

    std::deque<int32_t> queue;
    std::map<int32_t, Process> processesMap;

    for (const auto &process : processes) {
      processesMap[process.pid()] = process;
    }

    for (int32_t pid : queues[0]) {
      const auto &process = processesMap.at(pid);
      if (process.workTime() >= process.timer()) {
        queue.push_back(process.pid());
      }
    }

    std::stable_sort(queue.begin(), queue.end(),
                     [&processesMap](int32_t left, int32_t right) {
                       auto leftTime = processesMap[left].workTime() -
                                       processesMap[left].timer();
                       auto rightTime = processesMap[right].workTime() -
                                        processesMap[right].timer();

                       return leftTime < rightTime;
                     });

    for (int32_t pid : queues[0]) {
      const auto &process = processesMap.at(pid);
      if (process.workTime() < process.timer()) {
        queue.push_back(process.pid());
      }
    }

    queues[0] = queue;

    return {processes, queues};
  }
};
} // namespace ProcessesManagement
