#pragma once

#include <algorithm>
#include <cstdint>
#include <ctime>
#include <iterator>
#include <random>
#include <set>
#include <utility>
#include <vector>

#include <algo/memory/requests.h>
#include <algo/memory/strategies.h>
#include <algo/memory/types.h>
#include <utils/tasks.h>

namespace Generators::MemoryTask::Details {
using namespace MemoryManagement;
using std::set;
using std::vector;

struct MemoryBlockCmp {
  bool operator()(const Types::MemoryBlock &first,
                  const Types::MemoryBlock &second) const {
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

inline Strategies::StrategyPtr randStrategy() {
  auto strategyIndex = randRange(0, 2);
  switch (strategyIndex) {
  case 0:
    return MemoryManagement::Strategies::FirstAppropriateStrategy::create();
  case 1:
    return MemoryManagement::Strategies::MostAppropriateStrategy::create();
  case 2:
    return MemoryManagement::Strategies::LeastAppropriateStrategy::create();
  }
}

inline Requests::RequestType randRequestType() {
  vector<Requests::RequestType> types = {
      Requests::RequestType::CREATE_PROCESS,
      Requests::RequestType::CREATE_PROCESS,
      Requests::RequestType::ALLOCATE_MEMORY,
      Requests::RequestType::FREE_MEMORY,
      Requests::RequestType::TERMINATE_PROCESS,
      Requests::RequestType::TERMINATE_PROCESS};
  return types[randRange(0, 5)];
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

inline set<int32_t> getAvailablePids(const Types::MemoryState &state) {
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

inline set<int32_t> getUsedPids(const Types::MemoryState &state) {
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

inline Requests::RequestPtr genCreateProcess(const Types::MemoryState &state,
                                             bool valid = true) {
  const auto [blocks, freeBlocks] = state;
  auto usedPids = getUsedPids(state), availabePids = getAvailablePids(state);

  int32_t freePages = 0;
  for (const auto &block : freeBlocks) {
    freePages += block.size();
  }

  if (valid && !availabePids.empty() && freePages > 0) {
    auto [pages, bytes] = genRequestedMemory(freePages);

    auto newPid = randChoice(availabePids.begin(), availabePids.end());
    return Requests::CreateProcess::create(newPid, bytes, pages);
  } else {
    auto [pages, bytes] = genRequestedMemory(randRange(1, 255));

    auto newPid = randRange(0, 255);
    return Requests::CreateProcess::create(newPid, bytes, pages);
  }
}

inline Requests::RequestPtr genTerminateProcess(const Types::MemoryState &state,
                                                bool valid = true) {
  auto usedPids = getUsedPids(state), availabePids = getAvailablePids(state);
  if (valid && !usedPids.empty()) {
    auto pid = randChoice(usedPids.begin(), usedPids.end());
    return Requests::TerminateProcess::create(pid);
  } else if (usedPids.empty()) {
    auto pid = randChoice(availabePids.begin(), availabePids.end());
    return Requests::TerminateProcess::create(pid);
  } else {
    auto pid = randChoice(usedPids.begin(), usedPids.end());
    return Requests::TerminateProcess::create(pid);
  }
}

inline Requests::RequestPtr genAllocateMemory(const Types::MemoryState &state,
                                              bool valid = true) {
  const auto [blocks, freeBlocks] = state;
  auto usedPids = getUsedPids(state), availabePids = getAvailablePids(state);

  int32_t freePages = 0;
  for (const auto &block : freeBlocks) {
    freePages += block.size();
  }

  if (valid && !usedPids.empty() && freePages > 0) {
    auto [pages, bytes] = genRequestedMemory(freePages);

    auto newPid = randChoice(usedPids.begin(), usedPids.end());
    return Requests::AllocateMemory::create(newPid, bytes, pages);
  } else {
    auto [pages, bytes] = genRequestedMemory(randRange(1, 255));

    auto newPid = randRange(0, 255);
    return Requests::AllocateMemory::create(newPid, bytes, pages);
  }
}

inline Requests::RequestPtr genFreeMemory(const Types::MemoryState &state,
                                          bool valid = true) {
  const auto [blocks, freeBlocks] = state;
  auto usedPids = getUsedPids(state), availabePids = getAvailablePids(state);

  set<Types::MemoryBlock, MemoryBlockCmp> usedBlocks;
  for (const auto &block : blocks) {
    if (block.pid() != -1) {
      usedBlocks.insert(block);
    }
  }

  if (valid && !usedBlocks.empty()) {
    auto block = randChoice(usedBlocks.begin(), usedBlocks.end());
    return Requests::FreeMemory::create(block.pid(), block.address());
  } else {
    auto pid = randRange(0, 255);
    auto address = randRange(0, 255);
    return Requests::FreeMemory::create(pid, address);
  }
}
} // namespace Generators::MemoryTask::Details

namespace Generators::MemoryTask {
inline Utils::Tasks::MemoryTask generate(uint32_t requestCount = 40) {
  using namespace MemoryManagement::Requests;
  using namespace MemoryManagement::Types;
  using namespace Details;

  auto strategy = randStrategy();
  auto state = Types::MemoryState::initial();
  std::vector<RequestPtr> requests;

  for (uint32_t i = 0; i < requestCount; ++i) {
    RequestPtr newRequest;
    bool valid = randRange(0, 256) % 3 > 0;

    auto type = randRequestType();
    switch (type) {
    case RequestType::CREATE_PROCESS:
      newRequest = Details::genCreateProcess(state, valid);
      break;
    case RequestType::TERMINATE_PROCESS:
      newRequest = Details::genTerminateProcess(state, valid);
      break;
    case RequestType::ALLOCATE_MEMORY:
      newRequest = Details::genAllocateMemory(state, valid);
      break;
    case RequestType::FREE_MEMORY:
      newRequest = Details::genFreeMemory(state, valid);
      break;
    }
    requests.push_back(newRequest);
    state = strategy->processRequest(newRequest, state);
  }
  return Utils::Tasks::MemoryTask::create(strategy, 0, MemoryState::initial(),
                                          requests);
}
} // namespace Generators::MemoryTask
