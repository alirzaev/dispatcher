#pragma once

#include <QPoint>
#include <QWidget>

#include "models.h"
#include <algo/memory/types.h>

using MemoryManagement::MemoryBlock;

namespace Ui {
class MemoryTask;
}

class MemoryTask : public QWidget {
  Q_OBJECT

public:
  explicit MemoryTask(Models::MemoryModel model, QWidget *parent = nullptr);

  Models::MemoryModel model() const;

  ~MemoryTask() override;

private:
  Ui::MemoryTask *ui;

  Models::MemoryModel _model;

  void provideContextMenu(const QPoint &pos);

  MemoryManagement::MemoryState collectState();

  void processActionAllocate(const MemoryBlock &block, uint32_t blockIndex);

  void processActionFree(const MemoryBlock &block, uint32_t blockIndex);

  void processActionDefragment();

  void processActionCompress(uint32_t blockIndex);

  void refresh();

  void nextRequest();

  void setMemoryBlocks(const std::vector<MemoryBlock> &blocks);

  void setFreeMemoryBlocks(const std::vector<MemoryBlock> &blocks);

  void setRequest(const MemoryManagement::Request &request);

  void setStrategy(MemoryManagement::StrategyType type);

  void showErrorMessage(const std::string &message);

  void showInfoMessage(const std::string &message);
};
