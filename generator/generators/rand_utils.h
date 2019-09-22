#pragma once

#include <algorithm>
#include <ctime>
#include <random>
#include <type_traits>

namespace Generators::RandUtils {
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

template <class Container>
inline decltype(auto) randChoice(Container &&container) {
  return randChoice(std::cbegin(container), std::cend(container));
}

template <class Container>
inline void randShuffle(Container &container) {
  static std::mt19937 e(static_cast<unsigned int>(std::time(nullptr)));
  std::shuffle(std::begin(container), std::end(container), e);
}
} // namespace Generators::RandUtils
