#pragma once

#include <stdexcept>
#include <string>

namespace Utils {
class BaseException : public std::logic_error {
public:
  BaseException(const std::string &what_arg) : std::logic_error(what_arg) {}
};

class TaskException : public BaseException {
public:
  TaskException(const std::string &what_arg) : BaseException(what_arg) {}
};
} // namespace Utils
