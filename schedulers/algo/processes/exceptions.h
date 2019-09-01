#pragma once

#include <stdexcept>
#include <string>

namespace ProcessesManagement {
class BaseException : public std::logic_error {
public:
  BaseException(const std::string &what_arg) : std::logic_error(what_arg) {}
};

class RequestException : public BaseException {
public:
  RequestException(const std::string &what_arg) : BaseException(what_arg) {}
};

class OperationException : public BaseException {
public:
  OperationException(const std::string &what_arg) : BaseException(what_arg) {}
};

class TypeException : public BaseException {
public:
  TypeException(const std::string &what_arg) : BaseException(what_arg) {}
};
} // namespace ProcessesManagement
