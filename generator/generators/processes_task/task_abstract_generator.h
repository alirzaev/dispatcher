#pragma once

#include <cstdint>
#include <iterator>
#include <optional>
#include <set>
#include <utility>
#include <variant>
#include <vector>

#include <algo/processes/requests.h>
#include <algo/processes/strategies.h>
#include <algo/processes/types.h>
#include <utils/tasks.h>

namespace Generators::ProcessesTask::Details {
using namespace ProcessesManagement;
template <class T> bool sameType(const Request &v1, const Request &v2) {
  return std::holds_alternative<T>(v1) && std::holds_alternative<T>(v2);
}
} // namespace Generators::ProcessesTask::Details

namespace Generators::ProcessesTask::TaskGenerators {
using namespace ProcessesManagement;
using std::holds_alternative;
using std::optional;
using std::set;
using std::vector;

class AbstractTaskGenerator {
public:
  virtual ~AbstractTaskGenerator() {}

  AbstractTaskGenerator() = default;

  virtual vector<Request> generate(const ProcessesState &state,
                                   optional<Request> last,
                                   bool valid = true) const {
    using Details::sameType;
    namespace PM = ProcessesManagement;

    vector<optional<Request>> requests = {
        this->CreateProcessReq(state, valid),
        this->CreateProcessReq(state, valid),
        this->TerminateProcessReq(state, valid),
        this->InitIO(state, valid),
        this->InitIO(state, valid),
        this->TerminateIO(state, valid),
        this->TerminateIO(state, valid),
        this->TransferControl(state, valid),
        this->TimeQuantumExpired(state, valid)};

    vector<Request> filtered;
    if (!last.has_value()) {
      // первая заявка - только CreateProcessReq
      for (const auto &request : requests) {
        if (request.has_value() &&
            holds_alternative<PM::CreateProcessReq>(request.value())) {
          filtered.push_back(request.value());
        }
      }
    } else {
      auto lastVal = last.value();

      for (const auto &request : requests) {
        // не должно быть двух подряд идущих заявок TimeQuantumExpired или
        // TransferControl
        bool ok = request.has_value() &&
                  !sameType<PM::TimeQuantumExpired>(request.value(), lastVal) &&
                  !sameType<PM::TransferControl>(request.value(), lastVal);
        if (ok) {
          filtered.push_back(request.value());
        }
      }
    }
    return filtered;
  }

  virtual optional<Request> CreateProcessReq(const ProcessesState &state,
                                             bool valid = true) const = 0;

  virtual optional<Request> TerminateProcessReq(const ProcessesState &state,
                                                bool valid = true) const = 0;

  virtual optional<Request> InitIO(const ProcessesState &state,
                                   bool valid = true) const = 0;

  virtual optional<Request> TerminateIO(const ProcessesState &state,
                                        bool valid = true) const = 0;

  virtual optional<Request> TransferControl(const ProcessesState &state,
                                            bool valid = true) const = 0;

  virtual optional<Request> TimeQuantumExpired(const ProcessesState &,
                                               bool) const {
    return ProcessesManagement::TimeQuantumExpired();
  }

protected:
  int32_t maxPid() const { return 16; }

  inline set<int32_t> getAvailablePids(const ProcessesState &state) const {
    auto [processes, queues] = state;
    set<int32_t> existingPids, availabePids;
    for (const auto &process : processes) {
      existingPids.insert(process.pid());
    }
    for (int32_t pid = 0; pid < maxPid(); ++pid) {
      if (auto p = existingPids.find(pid); p == existingPids.end()) {
        availabePids.insert(pid);
      }
    }
    return availabePids;
  }

  inline set<int32_t> getUsedPids(const ProcessesState &state) const {
    auto [processes, queues] = state;
    set<int32_t> usedPids;
    for (const auto &process : processes) {
      usedPids.insert(process.pid());
    }
    return usedPids;
  }
};
} // namespace Generators::ProcessesTask::TaskGenerators
