#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <random>
#include <set>
#include <utility>
#include <vector>

#include <tl/optional.hpp>

#include <algo/memory/requests.h>
#include <algo/memory/strategies.h>
#include <algo/memory/types.h>
#include <utils/tasks.h>

#include "rand_utils.h"

namespace Generators::MemoryTask::Details {
using namespace MemoryManagement;
using std::set;
using std::vector;
using tl::optional;

struct MemoryBlockCmp {
  bool operator()(const MemoryBlock &first, const MemoryBlock &second) const {
    return first.size() < second.size();
  }
};

inline constexpr int32_t maxPid() { return 16; }

inline StrategyPtr randStrategy() {
  vector<MemoryManagement::StrategyPtr> strategies = {
      MemoryManagement::FirstAppropriateStrategy::create(),
      MemoryManagement::MostAppropriateStrategy::create(),
      MemoryManagement::LeastAppropriateStrategy::create()};

  return RandUtils::randChoice(strategies);
}

inline set<int32_t> getAvailablePids(const MemoryState &state) {
  auto [blocks, freeBlocks] = state;
  set<int32_t> existingPids, availabePids;
  for (const auto &block : blocks) {
    if (block.pid() != -1) {
      existingPids.insert(block.pid());
    }
  }
  for (int32_t pid = 0; pid < maxPid(); ++pid) {
    if (existingPids.find(pid) == existingPids.end()) {
      availabePids.insert(pid);
    }
  }
  return availabePids;
}

inline set<int32_t> getUsedPids(const MemoryState &state) {
  auto [blocks, freeBlocks] = state;
  set<int32_t> usedPids;
  for (const auto &block : blocks) {
    if (block.pid() != -1) {
      usedPids.insert(block.pid());
    }
  }
  return usedPids;
}

// <pages, bytes>
inline std::pair<int32_t, int32_t> genRequestedMemory(int32_t availablePages) {
  int32_t pages = RandUtils::randRange(1, availablePages);
  int32_t min = (pages - 1) * 4096 + 1;
  int32_t max = pages * 4096;
  return {pages, RandUtils::randRange(min, max)};
}

inline optional<Request> genCreateProcess(const MemoryState &state,
                                          bool valid = true) {
  using namespace RandUtils;

  auto [blocks, freeBlocks] = state;
  auto usedPids = getUsedPids(state), availabePids = getAvailablePids(state);

  int32_t freePages = 0;
  for (const auto &block : freeBlocks) {
    freePages += block.size();
  }

  if (valid && !availabePids.empty() && freePages > 0) {
    auto [pages, bytes] = genRequestedMemory(freePages);
    auto newPid = randChoice(availabePids);

    return CreateProcessReq(newPid, bytes, pages);
  } else if (!valid && !usedPids.empty()) {
    auto [pages, bytes] = genRequestedMemory(randRange(1, 255));
    auto newPid = randChoice(usedPids);

    return CreateProcessReq(newPid, bytes, pages);
  } else {
    return tl::nullopt;
  }
}

inline optional<Request> genTerminateProcess(const MemoryState &state,
                                             bool valid = true) {
  auto usedPids = getUsedPids(state), availabePids = getAvailablePids(state);
  if (valid && !usedPids.empty()) {
    auto pid = RandUtils::randChoice(usedPids);

    return TerminateProcessReq(pid);
  } else if (!valid && !availabePids.empty()) {
    auto pid = RandUtils::randChoice(availabePids);

    return TerminateProcessReq(pid);
  } else {
    return tl::nullopt;
  }
}

inline optional<Request> genAllocateMemory(const MemoryState &state,
                                           bool valid = true) {
  using namespace RandUtils;

  auto [blocks, freeBlocks] = state;
  auto usedPids = getUsedPids(state), availabePids = getAvailablePids(state);

  int32_t freePages = 0;
  for (const auto &block : freeBlocks) {
    freePages += block.size();
  }

  if (valid && !usedPids.empty() && freePages > 0) {
    auto [pages, bytes] = genRequestedMemory(freePages);
    auto newPid = randChoice(usedPids);

    return AllocateMemory(newPid, bytes, pages);
  } else if (!valid) {
    if (!availabePids.empty()) {
      auto [pages, bytes] = genRequestedMemory(randRange(1, 255));
      auto newPid = randChoice(availabePids);

      return AllocateMemory(newPid, bytes, pages);
    } else if (!usedPids.empty() && freePages < 255) {
      auto [pages, bytes] = genRequestedMemory(randRange(freePages, 255));
      auto newPid = randChoice(usedPids);

      return AllocateMemory(newPid, bytes, pages);
    } else {
      return tl::nullopt;
    }
  } else {
    return tl::nullopt;
  }
}

inline optional<Request> genFreeMemory(const MemoryState &state,
                                       bool valid = true) {
  using namespace RandUtils;

  auto [blocks, freeBlocks] = state;
  auto usedPids = getUsedPids(state), availabePids = getAvailablePids(state);

  std::map<int32_t, size_t> blocksCount;
  set<MemoryBlock, MemoryBlockCmp> usedBlocks;
  for (const auto &block : blocks) {
    if (block.pid() != -1) {
      blocksCount[block.pid()] += 1;
    }
  }

  // При освобождении последнего блока памяти должна создаваться заявка на
  // завершение процесса
  for (const auto &block : blocks) {
    if (block.pid() != -1 && blocksCount[block.pid()] > 1) {
      usedBlocks.insert(block);
    }
  }

  if (valid && !usedBlocks.empty()) {
    auto block = randChoice(usedBlocks);

    return FreeMemory(block.pid(), block.address());
  } else {
    if (!usedBlocks.empty()) {
      auto block = randChoice(usedBlocks);
      auto delta = randRange(1, 255);
      auto newPid = (block.pid() + delta) % maxPid();

      return FreeMemory(newPid, block.address());
    } else {
      auto pid = randRange(0, maxPid());
      auto address = randRange(0, 255);

      return FreeMemory(pid, address);
    }
  }
}
} // namespace Generators::MemoryTask::Details

namespace Generators::MemoryTask {
inline Utils::MemoryTask generate(uint32_t requestCount = 40) {
  using namespace Details;
  using namespace RandUtils;
  using GenPtr = std::function<optional<Request>(const MemoryState &, bool)>;

  vector<GenPtr> gens = {&Details::genCreateProcess,
                         &Details::genTerminateProcess,
                         &Details::genAllocateMemory,
                         &Details::genFreeMemory};

  auto strategy = randStrategy();
  auto state = MemoryState::initial();
  vector<Request> requests;

  for (uint32_t i = 0; i < requestCount; ++i) {
    bool validRequired = i == 0 ? true : randRange(0, 256) % 8 > 0;
    vector<Request> validRequests, invalidRequests;

    for (auto gen : gens) {
      auto valid = gen(state, true);
      if (valid) {
        validRequests.push_back(valid.value());
      }

      auto invalid = gen(state, false);
      if (invalid) {
        invalidRequests.push_back(invalid.value());
      }
    }

    if (validRequired && !validRequests.empty()) {
      requests.push_back(randChoice(validRequests));
    } else if (!validRequired && !invalidRequests.empty()) {
      requests.push_back(randChoice(invalidRequests));
    } else if (validRequired && validRequests.empty()) {
      requests.push_back(randChoice(invalidRequests));
    } else {
      requests.push_back(randChoice(validRequests));
    }

    state = strategy->processRequest(requests.back(), state);
  }
  return Utils::MemoryTask::create(
      strategy, 0, MemoryState::initial(), requests);
}
} // namespace Generators::MemoryTask
