#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <QLineEdit>
#include <QPoint>
#include <QSpinBox>
#include <QString>
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
  explicit ProcessesTask(const Models::ProcessesModel &task,
                         QWidget *parent = nullptr);

  Utils::Task task() const override;

  ~ProcessesTask() override;

private:
  using ProcessesList =
      decltype(ProcessesManagement::ProcessesState::processes);
  using QueuesLists = decltype(ProcessesManagement::ProcessesState::queues);

  /* Controller */

  Models::ProcessesModel _model;

  size_t currentRequest;

  QString currentActions;

  std::vector<QString> actions;

  std::vector<ProcessesManagement::ProcessesState> states;

  void processActionCreate();

  void processActionTerminate(size_t index);

  void processActionToExecuting(size_t index);

  void processActionToWaiting(size_t index);

  void processActionToActive(size_t index);

  void processActionDecrease(size_t index);

  void pushToQueue(int pid, size_t queue);

  int32_t popFromQueue(size_t queue);

  void updateCurrentRequest(int index);

  void updateQueuesLists(int);

  void nextRequest();

  void refresh();

  /* View */

  Ui::ProcessesTask *ui;

  void setupSignals();

  void lockUi();

  void unlockUi();

  ProcessesManagement::ProcessesState collectState();

  void provideContextMenu(const QPoint &pos);

  void pushToQueueHandler(QLineEdit *lineEdit, QSpinBox *spinBox);

  void popFromQueueHandler(QLineEdit *lineEdit, QSpinBox *spinBox);

  size_t mapRowToIndex(int row);

  void updateMainView(const ProcessesManagement::ProcessesState &state,
                      const ProcessesManagement::Request &request);

  void updateStatsView(size_t count, size_t total, uint32_t fails);

  void updateStrategyView(ProcessesManagement::StrategyType type);

  void updateHistoryView(const Utils::ProcessesTask &task,
                         const std::vector<QString> &actions);

  void updateCurrentActionsView(const Utils::ProcessesTask &task,
                                const QString &actions);

  void setProcessesList(const ProcessesList &processes);

  void setQueuesLists(const QueuesLists &queues);

  void setRequest(const ProcessesManagement::Request &request);

  void warning(const std::string &message);
};
