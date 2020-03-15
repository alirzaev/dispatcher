#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <QPoint>
#include <QString>
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
  // Controller

  Models::MemoryModel _model;

  std::size_t currentRequest;

  QString currentActions;

  std::vector<QString> actions;

  std::vector<MemoryManagement::MemoryState> states;

  void processActionAllocate(const MemoryManagement::MemoryBlock &block,
                             uint32_t blockIndex);

  void processActionFree(const MemoryManagement::MemoryBlock &block,
                         uint32_t blockIndex);

  void processActionDefragment();

  void processActionCompress(uint32_t blockIndex);

  void updateCurrentRequest(int index);

  void nextRequest();

  void refresh();

  // View

  Ui::MemoryTask *ui;

  void lockUi();

  void unlockUi();

  MemoryManagement::MemoryState collectState();

  void provideContextMenu(const QPoint &pos);

  void updateMainView(MemoryManagement::MemoryState state,
                      MemoryManagement::Request request);

  void updateStatsView(std::size_t count, std::size_t total, uint32_t fails);

  void updateStrategyView(MemoryManagement::StrategyType type);

  void updateHistoryView(Utils::MemoryTask task,
                         const std::vector<QString> &actions);

  void updateCurrentActionsView(Utils::MemoryTask task, const QString actions);

  void
  setMemoryBlocks(const std::vector<MemoryManagement::MemoryBlock> &blocks);

  void
  setFreeMemoryBlocks(const std::vector<MemoryManagement::MemoryBlock> &blocks);

  void setRequest(const MemoryManagement::Request &request);

  void warning(const std::string &message);
};
