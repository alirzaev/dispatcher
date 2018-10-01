#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace MemoryManagement {
    namespace Exceptions {
    class BaseException : public std::logic_error {
    public:
        BaseException(const std::string& what_arg) :
            std::logic_error(what_arg)
        {}
    };

    class RequestException : public BaseException {
    public:
        RequestException(const std::string& what_arg) :
            BaseException(what_arg)
        {}
    };

    class OperationException : public BaseException {
    public:
        OperationException(const std::string& what_arg) :
            BaseException(what_arg)
        {}
    };

    class StrategyException : public BaseException {
    public:
        StrategyException(const std::string& what_arg) :
            BaseException(what_arg)
        {}
    };
    }
}

#endif // EXCEPTIONS_H
