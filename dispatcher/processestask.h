#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <QLineEdit>
#include <QPoint>
#include <QSpinBox>
#include <QWidget>

#include <algo/processes/strategies.h>
#include <algo/processes/types.h>
#include <utils/tasks.h>

#include <processestablewidget.h>

#include "models.h"
#include "taskgetter.h"

namespace Ui {
class ProcessesTask;
}

class ProcessesTask : public QWidget, public TaskGetter {
  Q_OBJECT

public:
  explicit ProcessesTask(Models::ProcessesModel task,
                         QWidget *parent = nullptr);

  Utils::Task task() const override;

  ~ProcessesTask() override;

private:
  using ProcessesList =
      decltype(ProcessesManagement::ProcessesState::processes);
  using QueuesLists = decltype(ProcessesManagement::ProcessesState::queues);

  // Controller

  Models::ProcessesModel _model;

  std::size_t currentRequest;

  std::vector<ProcessesManagement::ProcessesState> states;

  void processActionCreate();

  void processActionTerminate(std::size_t index);

  void processActionToExecuting(std::size_t index);

  void processActionToWaiting(std::size_t index);

  void processActionToActive(std::size_t index);

  void pushToQueue(int pid, std::size_t queue);

  int32_t popFromQueue(std::size_t queue);

  void updateCurrentRequest(int index);

  void updateQueuesLists(int);

  void nextRequest();

  void refresh();

  // View

  Ui::ProcessesTask *ui;

  void setupSignals();

  void lockUi();

  void unlockUi();

  ProcessesManagement::ProcessesState collectState();

  void provideContextMenu(const QPoint &pos);

  void pushToQueueHandler(QLineEdit *lineEdit, QSpinBox *spinBox);

  void popFromQueueHandler(QLineEdit *lineEdit, QSpinBox *spinBox);

  std::size_t mapRowToIndex(int row);

  void updateMainView(ProcessesManagement::ProcessesState state,
                      ProcessesManagement::Request request);

  void updateStatsView(std::size_t count, std::size_t total, uint32_t fails);

  void updateStrategyView(ProcessesManagement::StrategyType type);

  void updateHistoryView(Utils::ProcessesTask task);

  void setProcessesList(const ProcessesList &processes);

  void setQueuesLists(const QueuesLists &queues);

  void setRequest(const ProcessesManagement::Request &request);

  void warning(const std::string &message);
};
