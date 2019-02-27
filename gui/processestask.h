#pragma once

#include <string>

#include <QLineEdit>
#include <QPoint>
#include <QWidget>

#include <algo/processes/strategies.h>
#include <algo/processes/types.h>

#include "models.h"
#include "widgets/processestablewidget.h"

namespace Ui {
class ProcessesTask;
}

class ProcessesTask : public QWidget {
  Q_OBJECT

public:
  explicit ProcessesTask(Models::ProcessesModel model,
                         QWidget *parent = nullptr);

  Models::ProcessesModel model() const;

  ~ProcessesTask() override;

private:
  using ProcessesList =
      decltype(ProcessesManagement::ProcessesState::processes);
  using QueuesLists = decltype(ProcessesManagement::ProcessesState::queues);

  Ui::ProcessesTask *ui;

  Models::ProcessesModel _model;

  ProcessesTableWidget *_processes;

  void connectAll();

  void provideContextMenu(const QPoint &pos);

  ProcessesManagement::ProcessesState collectState();

  void processActionCreate();

  void processActionTerminate(int row);

  void processActionToExecuting(int row);

  void processActionToWaiting(int row);

  void processActionToActive(int row);

  void refresh();

  void nextRequest();

  void setProcessesList(const ProcessesList &processes);

  void setQueuesLists(const QueuesLists &queues);

  void setRequest(const ProcessesManagement::Request &request);

  void setStrategy(ProcessesManagement::StrategyType type);

  void pushToQueue(int queue, int pid);

  void popFromQueue(int queue, QLineEdit *lineEdit);

  void showErrorMessage(const std::string &message);

  void showInfoMessage(const std::string &message);
};
