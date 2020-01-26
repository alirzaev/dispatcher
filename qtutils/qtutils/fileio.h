#pragma once

#ifdef _WIN32
#include <filesystem>
#endif

#include <fstream>
#include <memory>
#include <utility>

#include <QByteArray>
#include <QFile>
#include <QString>

namespace QtUtils::FileIO {
inline std::ifstream openStdIfstream(const QString &path) {
  std::ifstream file;
#ifdef _WIN32
  file.open(std::filesystem::path(path.toStdU16String()));
#else
  file.open(path.toStdString());
#endif
  return file;
}

inline std::ofstream openStdOfstream(const QString &path) {
  std::ofstream file;
  file.exceptions(std::ios_base::failbit);
#ifdef _WIN32
  file.open(std::filesystem::path(path.toStdU16String()));
#else
  file.open(path.toStdString());
#endif
  return file;
}

inline std::string readAll(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return std::string{};
  }

  return file.readAll().toStdString();
}
} // namespace QtUtils::FileIO
