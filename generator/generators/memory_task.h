#pragma once

#include <algorithm>
#include <cstdint>
#include <ctime>
#include <iterator>
#include <map>
#include <random>
#include <set>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <vector>

#include <algo/memory/requests.h>
#include <algo/memory/strategies.h>
#include <algo/memory/types.h>
#include <utils/tasks.h>

namespace Generators::MemoryTask::Details {
using namespace MemoryManagement;
using std::set;
using std::type_index;
using std::vector;

struct MemoryBlockCmp {
  bool operator()(const MemoryBlock &first, const MemoryBlock &second) const {
    return first.size() < second.size();
  }
};

inline int32_t randRange(int32_t a, int32_t b) {
  if (a > b) {
    std::swap(a, b);
  }
  static std::mt19937 e(std::time(nullptr));
  std::uniform_int_distribution<int32_t> dist(a, b);
  return dist(e);
}

template <class BidIt>
inline auto randChoice(BidIt first, BidIt last) -> decltype(*first) {
  auto size = std::distance(first, last);
  auto index = randRange(0, size - 1);
  auto it = first;
  for (decltype(index) i = 0; i < index; ++i) {
    ++it;
  }
  return *it;
}

inline StrategyPtr randStrategy() {
  vector<MemoryManagement::StrategyPtr> strategies = {
      MemoryManagement::FirstAppropriateStrategy::create(),
      MemoryManagement::MostAppropriateStrategy::create(),
      MemoryManagement::LeastAppropriateStrategy::create()};

  return randChoice(strategies.begin(), strategies.end());
}

inline std::type_index randRequestType() {
  auto types = {type_index(typeid(CreateProcessReq)),
                type_index(typeid(CreateProcessReq)),
                type_index(typeid(AllocateMemory)),
                type_index(typeid(FreeMemory)),
                type_index(typeid(TerminateProcessReq)),
                type_index(typeid(TerminateProcessReq))};
  return randChoice(types.begin(), types.end());
}

inline set<int32_t> getAvailablePids(const MemoryState &state) {
  const auto [blocks, freeBlocks] = state;
  set<int32_t> existingPids, availabePids;
  for (const auto &block : blocks) {
    if (block.pid() != -1) {
      existingPids.insert(block.pid());
    }
  }
  for (int32_t pid = 0; pid < 256; ++pid) {
    if (auto p = existingPids.find(pid); p == existingPids.end()) {
      availabePids.insert(pid);
    }
  }
  return availabePids;
}

inline set<int32_t> getUsedPids(const MemoryState &state) {
  const auto [blocks, freeBlocks] = state;
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
  int32_t pages = randRange(1, availablePages);
  int32_t min = (pages - 1) * 4096 + 1;
  int32_t max = pages * 4096;
  return {pages, randRange(min, max)};
}

inline Request genCreateProcess(const MemoryState &state, bool valid = true) {
  const auto [blocks, freeBlocks] = state;
  auto usedPids = getUsedPids(state), availabePids = getAvailablePids(state);

  int32_t freePages = 0;
  for (const auto &block : freeBlocks) {
    freePages += block.size();
  }

  if (valid && !availabePids.empty() && freePages > 0) {
    auto [pages, bytes] = genRequestedMemory(freePages);

    auto newPid = randChoice(availabePids.begin(), availabePids.end());
    return CreateProcessReq(newPid, bytes, pages);
  } else {
    auto [pages, bytes] = genRequestedMemory(randRange(1, 255));

    auto newPid = randRange(0, 255);
    return CreateProcessReq(newPid, bytes, pages);
  }
}

inline Request genTerminateProcess(const MemoryState &state,
                                   bool valid = true) {
  auto usedPids = getUsedPids(state), availabePids = getAvailablePids(state);
  if (valid && !usedPids.empty()) {
    auto pid = randChoice(usedPids.begin(), usedPids.end());
    return TerminateProcessReq(pid);
  } else if (usedPids.empty()) {
    auto pid = randChoice(availabePids.begin(), availabePids.end());
    return TerminateProcessReq(pid);
  } else {
    auto pid = randChoice(usedPids.begin(), usedPids.end());
    return TerminateProcessReq(pid);
  }
}

inline Request genAllocateMemory(const MemoryState &state, bool valid = true) {
  const auto [blocks, freeBlocks] = state;
  auto usedPids = getUsedPids(state), availabePids = getAvailablePids(state);

  int32_t freePages = 0;
  for (const auto &block : freeBlocks) {
    freePages += block.size();
  }

  if (valid && !usedPids.empty() && freePages > 0) {
    auto [pages, bytes] = genRequestedMemory(freePages);

    auto newPid = randChoice(usedPids.begin(), usedPids.end());
    return AllocateMemory(newPid, bytes, pages);
  } else {
    auto [pages, bytes] = genRequestedMemory(randRange(1, 255));

    auto newPid = randRange(0, 255);
    return AllocateMemory(newPid, bytes, pages);
  }
}

inline Request genFreeMemory(const MemoryState &state, bool valid = true) {
  const auto [blocks, freeBlocks] = state;
  auto usedPids = getUsedPids(state), availabePids = getAvailablePids(state);

  set<MemoryBlock, MemoryBlockCmp> usedBlocks;
  for (const auto &block : blocks) {
    if (block.pid() != -1) {
      usedBlocks.insert(block);
    }
  }

  if (valid && !usedBlocks.empty()) {
    auto block = randChoice(usedBlocks.begin(), usedBlocks.end());
    return FreeMemory(block.pid(), block.address());
  } else {
    auto pid = randRange(0, 255);
    auto address = randRange(0, 255);
    return FreeMemory(pid, address);
  }
}
} // namespace Generators::MemoryTask::Details

namespace Generators::MemoryTask {
inline Utils::MemoryTask generate(uint32_t requestCount = 40) {
  using namespace Details;

  auto strategy = randStrategy();
  auto state = MemoryState::initial();
  std::vector<Request> requests;

  for (uint32_t i = 0; i < requestCount; ++i) {
    bool valid = randRange(0, 256) % 3 > 0;
    auto type = randRequestType();

    if (type == type_index(typeid(CreateProcessReq))) {
      requests.push_back(Details::genCreateProcess(state, valid));
    } else if (type == type_index(typeid(TerminateProcessReq))) {
      requests.push_back(Details::genTerminateProcess(state, valid));
    } else if (type == type_index(typeid(AllocateMemory))) {
      requests.push_back(Details::genAllocateMemory(state, valid));
    } else {
      requests.push_back(Details::genFreeMemory(state, valid));
    }

    state = strategy->processRequest(requests.back(), state);
  }
  return Utils::MemoryTask::create(strategy, 0, MemoryState::initial(),
                                   requests);
} // namespace Generators::MemoryTask
} // namespace Generators::MemoryTask
