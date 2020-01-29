#pragma once

#include <vector>

#include <QPoint>
#include <QVector>
#include <QWidget>

#include <algo/memory/requests.h>
#include <algo/memory/types.h>
#include <utils/tasks.h>

#include "abstracttaskbuilder.h"
#include "historynavigator.h"

namespace Ui {
class MemoryTaskBuilder;
}

class MemoryTaskBuilder : public AbstractTaskBuilder, public HistoryNavigator {
public:
  explicit MemoryTaskBuilder(const Utils::Task &task,
                             QWidget *parent = nullptr);

  ~MemoryTaskBuilder() override;

private:
  /* Controller */
  Utils::MemoryTask _task;

  QVector<MemoryManagement::MemoryState> states;

  void loadTask(const Utils::MemoryTask &task);

  void currentRequestChanged(int index);

  void processContextMenuAction(const QString &action, int requestIndex);

  tl::optional<MemoryManagement::Request>
  processAddRequestMenuAction(const QString &action);

  void selectCurrentRequest(int requestIndex);

  /* View */
  Ui::MemoryTaskBuilder *ui;

  void updateTaskView(const MemoryManagement::MemoryState &state,
                      const MemoryManagement::Request &request);

  void clearTaskView();

  void provideContextMenu(const QPoint &pos);

  void setMemoryState(const MemoryManagement::MemoryState &state);

  void setRequest(const MemoryManagement::Request &request);

  void setRequestsList(const std::vector<MemoryManagement::Request> &requests);

  void setStrategy(MemoryManagement::StrategyType type);

protected:
  /* HistoryNavigator interface */
  void loadTaskFromHistory(Utils::Task task) override;

  // AbstractTaskBuilder interface
public:
  QString strategy() override;

  // AbstractTaskBuilder interface
public:
  Utils::Task task() override;
};
