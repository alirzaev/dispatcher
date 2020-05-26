#pragma once

#include <QPoint>
#include <QWidget>

#include <tl/optional.hpp>

#include <algo/processes/requests.h>
#include <algo/processes/types.h>
#include <utils/tasks.h>

#include "abstracttaskbuilder.h"
#include "historynavigator.h"

namespace Ui {
class ProcessesTaskBuilder;
}

class ProcessesTaskBuilder : public AbstractTaskBuilder,
                             public HistoryNavigator {
public:
  explicit ProcessesTaskBuilder(const Utils::Task &task,
                                QWidget *parent = nullptr);

  ~ProcessesTaskBuilder() override;

private:
  using ProcessesList =
      decltype(ProcessesManagement::ProcessesState::processes);
  using QueuesLists = decltype(ProcessesManagement::ProcessesState::queues);

  /* Controller */

  Utils::ProcessesTask _task;

  std::vector<ProcessesManagement::ProcessesState> states;

  int currentRequest;

  void loadTask(const Utils::ProcessesTask &task);

  void queuesListsChanged(int);

  void currentRequestChanged(int index);

  void processContextMenuAction(const QString &action, int requestIndex);

  tl::optional<ProcessesManagement::Request>
  processAddRequestMenuAction(const QString &action);

  void selectCurrentRequest(int requestIndex);

  /* View */

  Ui::ProcessesTaskBuilder *ui;

  void updateTaskView(const ProcessesManagement::ProcessesState &state,
                      const ProcessesManagement::Request &request);

  void clearTaskView();

  void provideContextMenu(const QPoint &pos);

  void setProcessesList(const ProcessesList &processes);

  void setQueuesLists(const QueuesLists &queues);

  void setRequest(const ProcessesManagement::Request &request);

  void
  setRequestsList(const std::vector<ProcessesManagement::Request> &requests);

  void setStrategy(ProcessesManagement::StrategyType type);

protected:
  /* HistoryNavigator interface */
  void loadTaskFromHistory(const Utils::Task &task) override;

  /* AbstractTaskBuilder interface */
public:
  QString strategy() override;

  /* AbstractTaskBuilder interface */
public:
  Utils::Task task() override;
};
