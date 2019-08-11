#pragma once

#include <cstddef>
#include <string>

#include <QLineEdit>
#include <QPoint>
#include <QWidget>

#include <algo/processes/strategies.h>
#include <algo/processes/types.h>
#include <utils/tasks.h>

#include "models.h"
#include "taskgetter.h"

#include "widgets/processestablewidget.h"

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

  Ui::ProcessesTask *ui;

  Models::ProcessesModel _model;

  void connectAll();

  void provideContextMenu(const QPoint &pos);

  ProcessesManagement::ProcessesState collectState();

  std::size_t mapRowToIndex(int row);

  void processActionCreate();

  void processActionTerminate(std::size_t index);

  void processActionToExecuting(std::size_t index);

  void processActionToWaiting(std::size_t index);

  void processActionToActive(std::size_t index);

  void refresh();

  void nextRequest();

  void setProcessesList(const ProcessesList &processes);

  void setQueuesLists(const QueuesLists &queues);

  void setRequest(const ProcessesManagement::Request &request);

  void setStrategy(ProcessesManagement::StrategyType type);

  void pushToQueue(std::size_t queue, int pid);

  void popFromQueue(std::size_t queue, QLineEdit *lineEdit);

  void showErrorMessage(const std::string &message);

  void showInfoMessage(const std::string &message);
};
