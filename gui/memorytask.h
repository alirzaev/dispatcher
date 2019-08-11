#pragma once

#include <cstdint>
#include <string>

#include <QPoint>
#include <QWidget>

#include <algo/memory/requests.h>
#include <algo/memory/strategies.h>
#include <algo/memory/types.h>
#include <utils/tasks.h>

#include "models.h"
#include "taskgetter.h"

namespace Ui {
class MemoryTask;
}

class MemoryTask : public QWidget, public TaskGetter {
  Q_OBJECT

public:
  explicit MemoryTask(Models::MemoryModel task, QWidget *parent = nullptr);

  Utils::Task task() const override;

  ~MemoryTask() override;

private:
  Ui::MemoryTask *ui;

  Models::MemoryModel _model;

  void provideContextMenu(const QPoint &pos);

  MemoryManagement::MemoryState collectState();

  void processActionAllocate(const MemoryManagement::MemoryBlock &block,
                             uint32_t blockIndex);

  void processActionFree(const MemoryManagement::MemoryBlock &block,
                         uint32_t blockIndex);

  void processActionDefragment();

  void processActionCompress(uint32_t blockIndex);

  void refresh();

  void nextRequest();

  void
  setMemoryBlocks(const std::vector<MemoryManagement::MemoryBlock> &blocks);

  void
  setFreeMemoryBlocks(const std::vector<MemoryManagement::MemoryBlock> &blocks);

  void setRequest(const MemoryManagement::Request &request);

  void setStrategy(MemoryManagement::StrategyType type);

  void showErrorMessage(const std::string &message);

  void showInfoMessage(const std::string &message);
};
