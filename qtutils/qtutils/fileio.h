#pragma once

#ifdef _WIN32
#include <filesystem>
#endif

#include <fstream>
#include <memory>
#include <utility>

#include <QByteArray>
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

inline std::ofstream openStdOfstream(const QString &path, bool binary = false) {
  std::ofstream file;
  file.exceptions(std::ios_base::failbit);
  std::ios_base::openmode openmode = std::ios_base::out;
  if (binary) {
    openmode |= std::ios_base::binary;
  }
#ifdef _WIN32
  file.open(std::filesystem::path(path.toStdU16String()), openmode);
#else
  file.open(path.toStdString(), openmode);
#endif
  return file;
}

inline std::string readAll(const QString &path) {
  std::ifstream file;
  file.exceptions(std::ios_base::failbit);
  std::ios_base::openmode openmode = std::ios_base::binary | std::ios_base::ate;
#ifdef _WIN32
  file.open(std::filesystem::path(path.toStdU16String()), openmode);
#else
  file.open(path.toStdString(), openmode);
#endif
  auto size = file.tellg();
  std::string content(size, '\0');
  file.seekg(0);
  file.read(content.data(), size);
  return content;
}
} // namespace QtUtils::FileIO
