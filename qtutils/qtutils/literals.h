#pragma once

#include <cstddef>

#include <QString>

namespace QtUtils::Literals {
inline QString operator"" _qs(const char *str, size_t size) {
  return QString::fromUtf8(str, static_cast<int>(size));
}
} // namespace QtUtils::Literals
