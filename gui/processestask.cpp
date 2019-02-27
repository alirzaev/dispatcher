#include <cstddef>
#include <exception>
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

#include "processestask.h"
#include "ui_processestask.h"

using namespace std::string_literals;
using namespace ProcessesManagement;
using namespace Utils::Literals;

ProcessesTask::ProcessesTask(Models::ProcessesModel model, QWidget *parent)
    : QWidget(parent), ui(new Ui::ProcessesTask), _model(model),
      _processes(new ProcessesTableWidget()) {
  ui->setupUi(this);

  ui->gridLayout->addWidget(_processes, 1, 0, 1, 1);
  _processes->setContextMenuPolicy(Qt::CustomContextMenu);

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
  connect(ui->pushButtonQueue1Push, &QPushButton::clicked, this, [=]() {
    auto pid = ui->lineEditQueue1Push->text().toInt();
    auto queue = ui->spinBoxQueue1->value();

    pushToQueue(queue, pid);
  });
  connect(ui->pushButtonQueue2Push, &QPushButton::clicked, this, [=]() {
    auto pid = ui->lineEditQueue2Push->text().toInt();
    auto queue = ui->spinBoxQueue2->value();

    pushToQueue(queue, pid);
  });
  connect(ui->pushButtonQueue1Pop, &QPushButton::clicked, this, [=]() {
    auto queue = ui->spinBoxQueue1->value();

    popFromQueue(queue, ui->lineEditQueue1Pop);
  });
  connect(ui->pushButtonQueue2Pop, &QPushButton::clicked, this, [=]() {
    auto queue = ui->spinBoxQueue2->value();

    popFromQueue(queue, ui->lineEditQueue2Pop);
  });
  connect(_processes, &ProcessesTableWidget::customContextMenuRequested, this,
          &ProcessesTask::provideContextMenu);
}

void ProcessesTask::provideContextMenu(const QPoint &pos) {
  qDebug() << "ContextMenu";

  auto globalPos = _processes->mapToGlobal(pos);
  auto *item = _processes->itemAt(pos);
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
  auto queue2Index = static_cast<size_t>(ui->spinBoxQueue2->value());
  auto queue1Size = static_cast<size_t>(ui->listQueue1->count());
  auto queue2Size = static_cast<size_t>(ui->listQueue2->count());

  queues[queue1Index].resize(queue1Size);
  queues[queue2Index].resize(queue2Size);

  for (size_t i = 0; i < queue1Size; ++i) {
    queues[queue1Index][i] = ui->listQueue1->item(i)->text().toInt();
  }
  for (size_t i = 0; i < queue2Size; ++i) {
    queues[queue2Index][i] = ui->listQueue2->item(i)->text().toInt();
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
  setStrategy(_model.task.strategy()->type);
  if (_model.task.done()) {
    setRequest(_model.task.requests().back());
    showInfoMessage("Вы успешно выполнили данное задание");
  } else {
    auto index = _model.task.completed();
    setRequest(_model.task.requests()[index]);
  }
}

void ProcessesTask::nextRequest() {
  _model.state = collectState();
  auto task = _model.task.next(_model.state);
  if (task) {
    _model.task = task.value();
    _model.state = _model.task.state();
    refresh();
  } else {
    showErrorMessage("Заявка обработана неверно");
  }
}

void ProcessesTask::setProcessesList(const ProcessesList &processes) {
  _processes->setProcesses(processes);
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
  auto *label = ui->labelRequestDescr;

  std::visit(
      overload{
          [label](const CreateProcessReq &req) {
            if (req.ppid() == -1) {
              label->setText("Возник новый процесс PID = %1"_qs.arg(req.pid()));
            } else {
              label->setText(
                  "Процесс PID = %1 породил дочерний процесс PID = %2"_qs
                      .arg(req.ppid())
                      .arg(req.pid()));
            }
          },
          [label](const TerminateProcessReq &req) {
            label->setText("Завершен процесс PID = %1"_qs.arg(req.pid()));
          },
          [label](const InitIO &req) {
            label->setText("Процесс PID = %1 выдал запрос на ввод/вывод"_qs.arg(
                req.pid()));
          },
          [label](const TerminateIO &req) {
            label->setText(
                "Ввод/вывод для процесса PID = %1 завершен"_qs.arg(req.pid()));
          },
          [label](const TransferControl &req) {
            label->setText(
                "Процесс PID = %1 передал управление операционной системе"_qs
                    .arg(req.pid()));
          },
          [label](const TimeQuantumExpired &) {
            label->setText("Истек квант времени"_qs);
          }},
      request);
}

void ProcessesTask::setStrategy(StrategyType type) {
  auto *label = ui->strategyLabel;

  if (type == StrategyType::ROUNDROBIN) {
    label->setText("Стратегия: Round Robin"_qs);
  }
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
