#pragma once

#include <cstddef>

#include <QString>

namespace Utils::Literals {
inline QString operator"" _qs(const char *str, std::size_t size) {
  return QString::fromUtf8(str, static_cast<int>(size));
}
} // namespace Utils::Literals
