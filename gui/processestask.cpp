#include <cstddef>
#include <exception>
#include <map>
#include <string>
#include <variant>

#include <QDebug>
#include <QIntValidator>
#include <QLineEdit>
#include <QMessageBox>
#include <QPoint>
#include <QPushButton>
#include <QSpinBox>
#include <QString>

#include <algo/processes/exceptions.h>
#include <algo/processes/operations.h>
#include <algo/processes/requests.h>
#include <algo/processes/strategies.h>
#include <algo/processes/types.h>
#include <utils/overload.h>

#include "literals.h"
#include "models.h"

#include "dialogs/createprocessdialog.h"
#include "menus/processmenu.h"
#include "widgets/processestablewidget.h"
#include "widgets/queuelistwidget.h"

#include "processestask.h"
#include "ui_processestask.h"

using namespace std::string_literals;
using namespace ProcessesManagement;
using namespace Utils::Literals;

ProcessesTask::ProcessesTask(Models::ProcessesModel model, QWidget *parent)
    : QWidget(parent), ui(new Ui::ProcessesTask), _model(model) {
  ui->setupUi(this);

  ui->processesTable->setContextMenuPolicy(Qt::CustomContextMenu);

  ui->lineEditQueue1Push->setValidator(new QIntValidator(0, 255));
  ui->lineEditQueue2Push->setValidator(new QIntValidator(0, 255));

  connectAll();

  refresh();
}

Models::ProcessesModel ProcessesTask::model() const { return _model; }

ProcessesTask::~ProcessesTask() { delete ui; }

void ProcessesTask::connectAll() {

  connect(ui->acceptRequest, &QPushButton::clicked, this,
          &ProcessesTask::nextRequest);
  connect(ui->resetState, &QPushButton::clicked, this, [=]() {
    _model.state = _model.task.state();
    refresh();
  });
  connect(ui->spinBoxQueue1, QOverload<int>::of(&QSpinBox::valueChanged), this,
          [=](int) { this->setQueuesLists(_model.state.queues); });
  connect(ui->spinBoxQueue2, QOverload<int>::of(&QSpinBox::valueChanged), this,
          [=](int) { this->setQueuesLists(_model.state.queues); });
  auto push1Handler = [=]() {
    auto pid = ui->lineEditQueue1Push->text().toInt();
    auto queue = ui->spinBoxQueue1->value();

    pushToQueue(queue, pid);
  };
  auto push2Handler = [=]() {
    auto pid = ui->lineEditQueue2Push->text().toInt();
    auto queue = ui->spinBoxQueue2->value();

    pushToQueue(queue, pid);
  };
  connect(ui->pushButtonQueue1Push, &QPushButton::clicked, this, push1Handler);
  connect(ui->lineEditQueue1Push, &QLineEdit::returnPressed, this,
          push1Handler);
  connect(ui->pushButtonQueue2Push, &QPushButton::clicked, this, push2Handler);
  connect(ui->lineEditQueue2Push, &QLineEdit::returnPressed, this,
          push2Handler);
  connect(ui->pushButtonQueue1Pop, &QPushButton::clicked, this, [=]() {
    auto queue = ui->spinBoxQueue1->value();

    popFromQueue(queue, ui->lineEditQueue1Pop);
  });
  connect(ui->pushButtonQueue2Pop, &QPushButton::clicked, this, [=]() {
    auto queue = ui->spinBoxQueue2->value();

    popFromQueue(queue, ui->lineEditQueue2Pop);
  });
  connect(ui->processesTable, &ProcessesTableWidget::customContextMenuRequested,
          this, &ProcessesTask::provideContextMenu);
  connect(ui->listQueue1, &QueueListWidget::itemsOrderChanged, this, [=]() {
    _model.state = collectState();
    refresh();
  });
}

void ProcessesTask::provideContextMenu(const QPoint &pos) {
  qDebug() << "ContextMenu";

  auto *processes = ui->processesTable;

  auto globalPos = processes->mapToGlobal(pos);
  auto *item = processes->itemAt(pos);
  auto row = item ? item->row() : -1;

  qDebug() << "ContextMenu: row " << row;

  ProcessMenu menu;

  auto action = menu.exec(globalPos);
  if (!action) {
    return;
  }

  if (action->text() == ProcessMenu::CREATE) {
    qDebug() << "ContextMenu: create";
    processActionCreate();
  } else if (action->text() == ProcessMenu::TERMINATE && row != -1) {
    qDebug() << "ContextMenu: terminate";
    processActionTerminate(row);
  } else if (action->text() == ProcessMenu::TO_EXECUTING && row != -1) {
    qDebug() << "ContextMenu: to executing";
    processActionToExecuting(row);
  } else if (action->text() == ProcessMenu::TO_WAITING && row != -1) {
    qDebug() << "ContextMenu: to waiting";
    processActionToWaiting(row);
  } else if (action->text() == ProcessMenu::TO_ACTIVE && row != -1) {
    qDebug() << "ContextMenu: to active";
    processActionToActive(row);
  }
}

ProcessesState ProcessesTask::collectState() {
  auto [processes, queues] = _model.state;

  auto queue1Index = static_cast<size_t>(ui->spinBoxQueue1->value());
  auto queue1Size = static_cast<size_t>(ui->listQueue1->count());

  queues[queue1Index].resize(queue1Size);

  for (size_t i = 0; i < queue1Size; ++i) {
    queues[queue1Index][i] = ui->listQueue1->item(i)->text().toInt();
  }

  return {processes, queues};
}

void ProcessesTask::processActionCreate() {
  _model.state = collectState();
  auto dialog = CreateProcessDialog(_model.state.processes, this);
  auto res = dialog.exec();

  if (res == QDialog::Accepted) {
    try {
      auto process = dialog.data;
      _model.state = addProcess(_model.state, process);
      refresh();
    } catch (const std::exception &ex) {
      showErrorMessage("Неизвестная ошибка: "s + ex.what());
    }
  }
}

void ProcessesTask::processActionTerminate(int row) {
  try {
    _model.state = collectState();
    auto pid = _model.state.processes[row].pid();
    _model.state = terminateProcess(_model.state, pid);
    refresh();
  } catch (const OperationException &ex) {
    if (ex.what() == "NO_SUCH_PROCESS"s) {
      showErrorMessage("Процесс с таким PID не существует");
    } else {
      showErrorMessage("Неизвестная ошибка: "s + ex.what());
    }
  } catch (const std::exception &ex) {
    showErrorMessage("Неизвестная ошибка: "s + ex.what());
  }
}

void ProcessesTask::processActionToExecuting(int row) {
  try {
    _model.state = collectState();
    auto pid = _model.state.processes[row].pid();
    _model.state = switchTo(_model.state, pid);
    refresh();
  } catch (const OperationException &ex) {
    if (ex.what() == "NO_SUCH_PROCESS"s) {
      showErrorMessage("Процесс с таким PID не существует");
    } else if (ex.what() == "INVALID_STATE"s) {
      showErrorMessage("Процесс должен быть в состоянии готовности");
    } else {
      showErrorMessage("Неизвестная ошибка: "s + ex.what());
    }
  } catch (const std::exception &ex) {
    showErrorMessage("Неизвестная ошибка: "s + ex.what());
  }
}

void ProcessesTask::processActionToWaiting(int row) {
  try {
    _model.state = collectState();
    auto pid = _model.state.processes[row].pid();
    _model.state = changeProcessState(_model.state, pid, ProcState::WAITING);
    refresh();
  } catch (const OperationException &ex) {
    if (ex.what() == "NO_SUCH_PROCESS"s) {
      showErrorMessage("Процесс с таким PID не существует");
    } else {
      showErrorMessage("Неизвестная ошибка: "s + ex.what());
    }
  } catch (const std::exception &ex) {
    showErrorMessage("Неизвестная ошибка: "s + ex.what());
  }
}

void ProcessesTask::processActionToActive(int row) {
  try {
    _model.state = collectState();
    auto pid = _model.state.processes[row].pid();
    _model.state = changeProcessState(_model.state, pid, ProcState::ACTIVE);
    refresh();
  } catch (const OperationException &ex) {
    if (ex.what() == "NO_SUCH_PROCESS"s) {
      showErrorMessage("Процесс с таким PID не существует");
    } else {
      showErrorMessage("Неизвестная ошибка: "s + ex.what());
    }
  } catch (const std::exception &ex) {
    showErrorMessage("Неизвестная ошибка: "s + ex.what());
  }
}

void ProcessesTask::refresh() {
  auto [processes, queues] = _model.state;
  setProcessesList(processes);
  setQueuesLists(queues);
  setStrategy(_model.task.strategy()->type());
  if (_model.task.done()) {
    setRequest(_model.task.requests().back());
    showInfoMessage("Вы успешно выполнили данное задание");
  } else {
    auto index = _model.task.completed();
    setRequest(_model.task.requests()[index]);
  }
}

void ProcessesTask::nextRequest() {
  auto state = updateTimer(collectState());
  auto task = _model.task.next(state);
  if (task) {
    _model.task = task.value();
    _model.state = _model.task.state();
    refresh();
  } else {
    showErrorMessage("Заявка обработана неверно");
  }
}

void ProcessesTask::setProcessesList(const ProcessesList &processes) {
  ui->processesTable->setProcesses(processes);
}

void ProcessesTask::setQueuesLists(const QueuesLists &queues) {
  auto queue1 = static_cast<size_t>(ui->spinBoxQueue1->value());
  auto queue2 = static_cast<size_t>(ui->spinBoxQueue2->value());

  ui->listQueue1->clear();
  for (auto pid : queues[queue1]) {
    ui->listQueue1->addItem("%1"_qs.arg(pid));
  }
  ui->listQueue2->clear();
  for (auto pid : queues[queue2]) {
    ui->listQueue2->addItem("%1"_qs.arg(pid));
  }
}

void ProcessesTask::setRequest(const Request &request) {
  auto description = QString::fromStdString(
      _model.task.strategy()->getRequestDescription(request));
  ui->labelRequestDescr->setText(description);
}

void ProcessesTask::setStrategy(StrategyType type) {
  auto *label = ui->strategyLabel;
  std::map<StrategyType, QString> strategyMap = {
      {StrategyType::ROUNDROBIN, "Стратегия: Round Robin"_qs},
      {StrategyType::FCFS, "Стратегия: FCFS"_qs},
      {StrategyType::SJT, "Стратегия: SJT"_qs},
      {StrategyType::SRT, "Стратегия: SRT"_qs}};

  label->setText(strategyMap[type]);
}

void ProcessesTask::pushToQueue(int queue, int pid) {
  try {
    _model.state = collectState();
    _model.state = ProcessesManagement::pushToQueue(_model.state, queue, pid);
    refresh();
  } catch (const OperationException &ex) {
    if (ex.what() == "NO_SUCH_PROCESS"s) {
      showErrorMessage("Процесс с таким PID не существует");
    } else if (ex.what() == "ALREADY_IN_QUEUE"s) {
      showErrorMessage("Процесс с таким PID уже добавлен в одну из очередей");
    } else {
      showErrorMessage("Неизвестная ошибка: "s + ex.what());
    }
  } catch (const std::exception &ex) {
    showErrorMessage("Неизвестная ошибка: "s + ex.what());
  }
}

void ProcessesTask::popFromQueue(int queue, QLineEdit *lineEdit) {
  try {
    _model.state = collectState();
    const auto &q = _model.state.queues[queue];
    if (!q.empty()) {
      lineEdit->setText("%1"_qs.arg(q.front()));
    }
    _model.state = ProcessesManagement::popFromQueue(_model.state, queue);

    refresh();
  } catch (const OperationException &ex) {
    if (ex.what() == "EMPTY_QUEUE"s) {
      showErrorMessage("Очередь пуста");
    } else {
      showErrorMessage("Неизвестная ошибка: "s + ex.what());
    }
  } catch (const std::exception &ex) {
    showErrorMessage("Неизвестная ошибка: "s + ex.what());
  }
}

void ProcessesTask::showErrorMessage(const std::string &message) {
  QMessageBox::critical(this, "Ошибка", QString::fromStdString(message));
}

void ProcessesTask::showInfoMessage(const std::string &message) {
  QMessageBox::information(this, "Внимание", QString::fromStdString(message));
}
