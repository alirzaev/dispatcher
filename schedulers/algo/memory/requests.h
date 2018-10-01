#ifndef MEMORY_REQUESTS_H
#define MEMORY_REQUESTS_H

#include <string>
#include <cstdint>
#include <memory>
#include "exceptions.h"


namespace MemoryManagement {
    namespace Requests {
    enum class RequestType {
        CREATE_PROCESS,
        TERMINATE_PROCESS,
        ALLOCATE_MEMORY,
        FREE_MEMORY
    };

    class AbstractRequest {
    public:
        AbstractRequest(RequestType type) :
            type(type)
        {}

        const RequestType type;

        virtual std::string toString() const = 0;

        virtual ~AbstractRequest()
        {}
    };

    using RequestPtr = std::shared_ptr<AbstractRequest>;

    class CreateProcess final : public AbstractRequest {
    public:
        const int32_t pid;

        const int32_t bytes;

        const int32_t pages;

        std::string toString() const
        {
            return "CreateProcess";
        }

        CreateProcess& operator=(const CreateProcess& rhs) = default;

        static std::shared_ptr<CreateProcess> create(int32_t pid, int32_t bytes, int32_t pages)
        {
            if (pid < -1 || pid > 255) {
                throw Exceptions::RequestException("INVALID_PID");
            }
            if (bytes < 1 || bytes > 256 * 4096) {
                throw Exceptions::RequestException("INVALID_BYTES");
            }
            if (pages < 1 || pages > 256) {
                throw Exceptions::RequestException("INVALID_PAGES");
            }
            if (bytes < (pages - 1) * 4096 || bytes > (pages + 1) * 4096) {
                throw Exceptions::RequestException("INVALID_BYTES");
            }

            return std::shared_ptr<CreateProcess>(new CreateProcess(pid, bytes, pages));
        }
    private:
        CreateProcess(int32_t pid, int32_t bytes, int32_t pages) :
            AbstractRequest(RequestType::CREATE_PROCESS),
            pid(pid), bytes(bytes), pages(pages)
        {}
    };

    class TerminateProcess final : public AbstractRequest {
    public:
        const int32_t pid;


        std::string toString() const {
            return "TerminateProcess";
        }

        TerminateProcess& operator=(const TerminateProcess& rhs) = default;

        static std::shared_ptr<TerminateProcess> create(int32_t pid)
        {
            if (pid < -1 || pid > 255) {
                throw Exceptions::RequestException("INVALID_PID");
            }

            return std::shared_ptr<TerminateProcess>(new TerminateProcess(pid));
        }
    private:
        TerminateProcess(int32_t pid) :
            AbstractRequest(RequestType::TERMINATE_PROCESS),
            pid(pid)
        {}
    };

    class AllocateMemory final : public AbstractRequest {
    public:
        const int32_t pid;

        const int32_t bytes;

        const int32_t pages;

        std::string toString() const {
            return "AllocateMemory";
        }

        AllocateMemory& operator=(const AllocateMemory& rhs) = default;

        static std::shared_ptr<AllocateMemory> create(int32_t pid, int32_t bytes, int32_t pages)
        {
            if (pid < -1 || pid > 255) {
                throw Exceptions::RequestException("INVALID_PID");
            }
            if (bytes < 1 || bytes > 256 * 4096) {
                throw Exceptions::RequestException("INVALID_BYTES");
            }
            if (pages < 1 || pages > 256) {
                throw Exceptions::RequestException("INVALID_PAGES");
            }
            if (bytes < (pages - 1) * 4096 || bytes > (pages + 1) * 4096) {
                throw Exceptions::RequestException("INVALID_BYTES");
            }

            return std::shared_ptr<AllocateMemory>(new AllocateMemory(pid, bytes, pages));
        }
    private:
        AllocateMemory(int32_t pid, int32_t bytes, int32_t pages) :
            AbstractRequest(RequestType::ALLOCATE_MEMORY),
            pid(pid), bytes(bytes), pages(pages)
        {}
    };

    class FreeMemory final : public AbstractRequest {
    public:
        const int32_t pid;

        const int32_t address;

        std::string toString() const {
            return "FreeMemory";
        }

        FreeMemory& operator=(const FreeMemory& rhs) = default;

        static std::shared_ptr<FreeMemory> create(int32_t pid, int32_t address)
        {
            if (pid < -1 || pid > 255) {
                throw Exceptions::RequestException("INVALID_PID");
            }
            if (address < 0 || address > 255) {
                throw Exceptions::RequestException("INVALID_ADDRESS");
            }

            return std::shared_ptr<FreeMemory>(new FreeMemory(pid, address));
        }
    private:
        FreeMemory(int32_t pid, int32_t address) :
            AbstractRequest(RequestType::FREE_MEMORY),
            pid(pid), address(address)
        {}
    };
    }
}

#endif // MEMORY_REQUESTS_H
