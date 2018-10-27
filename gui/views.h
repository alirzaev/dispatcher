#pragma once

#include <functional>
#include <vector>
#include <variant>
#include <string>
#include <tuple>

#include "../schedulers/utils/tasks.h"
#include "../schedulers/algo/memory/types.h"
#include "../schedulers/algo/memory/requests.h"

namespace Views {
class MemoryTaskView
{
public:
    using OnAllocateActionListener = std::function<
        void(const std::tuple<int32_t, int32_t, uint32_t>&, // pid, size, blockIndex
             const MemoryManagement::Types::MemoryState&)
    >;

    using OnFreeActionListener = std::function<
        void(const std::tuple<int32_t, uint32_t>&, // pid, blockIndex
             const MemoryManagement::Types::MemoryState&)
    >;

    using OnDefragmentActionListener = std::function<
        void(const MemoryManagement::Types::MemoryState&)
    >;

    using OnCompressActionListener = std::function<
        void(uint32_t, // blockIndex
             const MemoryManagement::Types::MemoryState&)
    >;

    virtual void onAllocateAction(OnAllocateActionListener listener) = 0;

    virtual void onFreeAction(OnFreeActionListener listener) = 0;

    virtual void onDefragmentAction(OnDefragmentActionListener listener) = 0;

    virtual void onCompressAction(OnCompressActionListener listener) = 0;

    virtual void setMemoryBlocks(const std::vector<MemoryManagement::Types::MemoryBlock>& blocks) = 0;

    virtual void setFreeMemoryBlocks(const std::vector<MemoryManagement::Types::MemoryBlock>& blocks) = 0;

    virtual void setRequest(MemoryManagement::Requests::RequestPtr request) = 0;

    virtual void showErrorMessage(const std::string& message) = 0;

    virtual ~MemoryTaskView() = default;
};

class ProcessTaskView
{ };

using TaskView = std::variant<MemoryTaskView*, ProcessTaskView*>;

class MainWindowView
{
public:
    using OnOpenListener = std::function<void(const std::string&)>;

    using OnSaveListener = std::function<void(const std::string&)>;

    MainWindowView() = default;

    virtual void onOpenTaskListener(OnOpenListener listener) = 0;

    virtual void onSaveTaskListener(OnSaveListener listener) = 0;

    virtual std::vector<TaskView>
    createTaskViews(const std::vector<Utils::Tasks::Task>& tasks) = 0;

    virtual ~MainWindowView() = default;
};
}
