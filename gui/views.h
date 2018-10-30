#pragma once

#include <functional>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include <algo/memory/requests.h>
#include <algo/memory/types.h>
#include <utils/tasks.h>

namespace Views {
using MemoryManagement::Types::MemoryBlock;
using MemoryManagement::Types::MemoryState;
using std::function;
using std::tuple;
using std::vector;

class MemoryTaskView {
public:
  using OnAllocateActionListener = function<void(
      const tuple<int32_t, int32_t, uint32_t> &, // pid, size, blockIndex
      const MemoryState &)>;

  using OnFreeActionListener =
      function<void(const tuple<int32_t, uint32_t> &, // pid, blockIndex
                    const MemoryState &)>;

  using OnDefragmentActionListener = function<void(const MemoryState &)>;

  using OnCompressActionListener = function<void(uint32_t, // blockIndex
                                                 const MemoryState &)>;

  using OnNextRequestListener = function<void(const MemoryState &)>;

  using OnResetStateListener = function<void()>;

  virtual void onAllocateAction(OnAllocateActionListener listener) = 0;

  virtual void onFreeAction(OnFreeActionListener listener) = 0;

  virtual void onDefragmentAction(OnDefragmentActionListener listener) = 0;

  virtual void onCompressAction(OnCompressActionListener listener) = 0;

  virtual void onNextRequestListener(OnNextRequestListener listener) = 0;

  virtual void onResetStateListener(OnResetStateListener listener) = 0;

  virtual void setMemoryBlocks(const vector<MemoryBlock> &blocks) = 0;

  virtual void setFreeMemoryBlocks(const vector<MemoryBlock> &blocks) = 0;

  virtual void setRequest(MemoryManagement::Requests::RequestPtr request) = 0;

  virtual void setStrategy(MemoryManagement::Strategies::StrategyType type) = 0;

  virtual void showErrorMessage(const std::string &message) = 0;

  virtual void showInfoMessage(const std::string &message) = 0;

  virtual ~MemoryTaskView() = default;
};

class ProcessTaskView {};

using TaskView = std::variant<MemoryTaskView *, ProcessTaskView *>;

class MainWindowView {
public:
  using OnOpenListener = function<void(const std::string &)>;

  using OnSaveListener = function<void(const std::string &)>;

  MainWindowView() = default;

  virtual void onOpenTaskListener(OnOpenListener listener) = 0;

  virtual void onSaveTaskListener(OnSaveListener listener) = 0;

  virtual std::vector<TaskView>
  createTaskViews(const std::vector<Utils::Tasks::Task> &tasks) = 0;

  virtual ~MainWindowView() = default;
};
} // namespace Views
