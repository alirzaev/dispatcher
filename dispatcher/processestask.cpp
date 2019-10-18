#include <cstddef>
#include <exception>
#include <map>
#include <string>

#include <QDebug>
#include <QFlags>
#include <QIntValidator>
#include <QLineEdit>
#include <QMessageBox>
#include <QPoint>
#include <QPushButton>
#include <QSpinBox>
#include <QString>

#include <mapbox/variant.hpp>
#include <tl/optional.hpp>

#include <algo/processes/exceptions.h>
#include <algo/processes/helpers.h>
#include <algo/processes/operations.h>
#include <algo/processes/requests.h>
#include <algo/processes/strategies.h>
#include <algo/processes/types.h>

#include <qtutils/literals.h>

#include <dialogs/createprocessdialog.h>
#include <menus/processmenu.h>
#include <processestablewidget.h>
#include <reorderablelistwidget.h>

#include "models.h"

#include "fontscale.h"

#include "processestask.h"
#include "ui_processestask.h"

using namespace std::string_literals;
using namespace ProcessesManagement;
using namespace QtUtils::Literals;

ProcessesTask::ProcessesTask(Models::ProcessesModel model, QWidget *parent)
    : QWidget(parent), ui(new Ui::ProcessesTask), _model(model) {
  ui->setupUi(this);

  auto font = QApplication::font();
  font.setPointSizeF(font.pointSizeF() * FONT_SCALE_FACTOR);
  ui->labelRequestDescr->setFont(font);

  ui->processesTable->setContextMenuPolicy(Qt::CustomContextMenu);

  ui->lineEditQueue1Push->setValidator(new QIntValidator(0, 255));
  ui->lineEditQueue2Push->setValidator(new QIntValidator(0, 255));

  connectAll();

  refresh();
}

Utils::Task ProcessesTask::task() const { return _model.task; }

ProcessesTask::~ProcessesTask() { delete ui; }

void ProcessesTask::connectAll() {

  connect(ui->acceptRequest,
          &QPushButton::clicked,
          this,
          &ProcessesTask::nextRequest);
  connect(ui->resetState, &QPushButton::clicked, this, [=]() {
    _model.state = _model.task.state();
    refresh();
  });
  connect(ui->spinBoxQueue1,
          QOverload<int>::of(&QSpinBox::valueChanged),
          this,
          [=](int) { this->setQueuesLists(_model.state.queues); });
  connect(ui->spinBoxQueue2,
          QOverload<int>::of(&QSpinBox::valueChanged),
          this,
          [=](int) { this->setQueuesLists(_model.state.queues); });
  auto push1Handler = [=]() {
    pushToQueue(ui->lineEditQueue1Push, ui->spinBoxQueue1);
  };
  auto push2Handler = [=]() {
    pushToQueue(ui->lineEditQueue2Push, ui->spinBoxQueue2);
  };
  connect(ui->pushButtonQueue1Push, &QPushButton::clicked, this, push1Handler);
  connect(
      ui->lineEditQueue1Push, &QLineEdit::returnPressed, this, push1Handler);
  connect(ui->pushButtonQueue2Push, &QPushButton::clicked, this, push2Handler);
  connect(
      ui->lineEditQueue2Push, &QLineEdit::returnPressed, this, push2Handler);
  connect(ui->pushButtonQueue1Pop, &QPushButton::clicked, this, [=]() {
    auto queue = static_cast<std::size_t>(ui->spinBoxQueue1->value());

    popFromQueue(queue, ui->lineEditQueue1Pop);
  });
  connect(ui->pushButtonQueue2Pop, &QPushButton::clicked, this, [=]() {
    auto queue = static_cast<std::size_t>(ui->spinBoxQueue2->value());

    popFromQueue(queue, ui->lineEditQueue2Pop);
  });
  connect(ui->processesTable,
          &ProcessesTableWidget::customContextMenuRequested,
          this,
          &ProcessesTask::provideContextMenu);
  connect(
      ui->listQueue1, &ReorderableListWidget::itemsOrderChanged, this, [=]() {
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
  _model.state = collectState();
  auto process =
      row != -1 ? tl::optional{_model.state.processes.at(mapRowToIndex(row))}
                : tl::nullopt;

  qDebug() << "ContextMenu: row " << row;

  ProcessMenu menu(process);

  auto action = menu.exec(globalPos);
  if (!action) {
    return;
  }

  if (action->text() == ProcessMenu::CREATE) {
    qDebug() << "ContextMenu: create";
    processActionCreate();
  } else if (action->text() == ProcessMenu::TERMINATE && row != -1) {
    qDebug() << "ContextMenu: terminate";
    processActionTerminate(mapRowToIndex(row));
  } else if (action->text() == ProcessMenu::TO_EXECUTING && row != -1) {
    qDebug() << "ContextMenu: to executing";
    processActionToExecuting(mapRowToIndex(row));
  } else if (action->text() == ProcessMenu::TO_WAITING && row != -1) {
    qDebug() << "ContextMenu: to waiting";
    processActionToWaiting(mapRowToIndex(row));
  } else if (action->text() == ProcessMenu::TO_ACTIVE && row != -1) {
    qDebug() << "ContextMenu: to active";
    processActionToActive(mapRowToIndex(row));
  }
}

ProcessesState ProcessesTask::collectState() {
  auto [processes, queues] = _model.state;

  auto queue1Index = static_cast<size_t>(ui->spinBoxQueue1->value());
  auto queue1Size = static_cast<size_t>(ui->listQueue1->count());

  queues[queue1Index].resize(queue1Size);

  for (size_t i = 0; i < queue1Size; ++i) {
    auto itemIndex = static_cast<int>(i);
    queues[queue1Index][i] = ui->listQueue1->item(itemIndex)->text().toInt();
  }

  return {processes, queues};
}

std::size_t ProcessesTask::mapRowToIndex(int row) {
  auto pid = ui->processesTable->item(row, 0)->text().toInt();

  return *getIndexByPid(_model.state.processes, pid);
}

void ProcessesTask::processActionCreate() {
  using Field = CreateProcessDialog::EditableField;

  _model.state = collectState();

  std::map<StrategyType, QFlags<Field>> flagsMap = {
      {StrategyType::ROUNDROBIN, {Field::Pid, Field::Ppid}},
      {StrategyType::LINUXO1, {Field::Pid, Field::Ppid}},
      {StrategyType::FCFS, {Field::Pid, Field::Ppid}},
      {StrategyType::SJN, {Field::Pid, Field::Ppid, Field::WorkTime}},
      {StrategyType::SRT, {Field::Pid, Field::Ppid, Field::WorkTime}},
      {StrategyType::UNIX, {Field::Pid, Field::Ppid, Field::Priority}},
      {StrategyType::WINDOWS,
       {Field::Pid, Field::Ppid, Field::Priority, Field::BasePriority}}};

  auto process = CreateProcessDialog::getProcess(
      this,
      _model.state.processes,
      flagsMap.at(_model.task.strategy()->type()));

  if (!process) {
    return;
  }

  try {
    _model.state = addProcess(_model.state, *process);
    refresh();
  } catch (const std::exception &ex) {
    warning("Неизвестная ошибка: "s + ex.what());
  }
}

void ProcessesTask::processActionTerminate(std::size_t index) {
  try {
    _model.state = collectState();
    auto pid = _model.state.processes.at(index).pid();
    _model.state = terminateProcess(_model.state, pid, false);
    refresh();
  } catch (const std::exception &ex) {
    warning("Неизвестная ошибка: "s + ex.what());
  }
}

void ProcessesTask::processActionToExecuting(std::size_t index) {
  try {
    _model.state = collectState();
    auto pid = _model.state.processes.at(index).pid();
    _model.state = switchTo(_model.state, pid);
    refresh();
  } catch (const std::exception &ex) {
    warning("Неизвестная ошибка: "s + ex.what());
  }
}

void ProcessesTask::processActionToWaiting(std::size_t index) {
  try {
    _model.state = collectState();
    auto pid = _model.state.processes.at(index).pid();
    _model.state = changeProcessState(_model.state, pid, ProcState::WAITING);
    refresh();
  } catch (const std::exception &ex) {
    warning("Неизвестная ошибка: "s + ex.what());
  }
}

void ProcessesTask::processActionToActive(std::size_t index) {
  try {
    _model.state = collectState();
    auto pid = _model.state.processes.at(index).pid();
    _model.state = changeProcessState(_model.state, pid, ProcState::ACTIVE);
    refresh();
  } catch (const std::exception &ex) {
    warning("Неизвестная ошибка: "s + ex.what());
  }
}

void ProcessesTask::refresh() {
  auto [processes, queues] = _model.state;
  setProcessesList(processes);
  setQueuesLists(queues);
  setStrategy(_model.task.strategy()->type());
  setCompletedTaskCount(_model.task.completed(), _model.task.requests().size());
  if (_model.task.done()) {
    setRequest(_model.task.requests().back());
  } else {
    auto index = _model.task.completed();
    setRequest(_model.task.requests()[index]);
  }
}

void ProcessesTask::nextRequest() {
  auto state = updateTimer(collectState());

  if (auto [ok, task] = _model.task.next(state); ok) {
    _model.task = task;
    _model.state = _model.task.state();
    refresh();
    if (_model.task.done()) {
      QMessageBox::information(
          this, "Внимание", "Вы успешно выполнили данное задание");
    }
  } else {
    warning("Заявка обработана неверно");
  }
}

void ProcessesTask::setProcessesList(const ProcessesList &processes) {
  ui->processesTable->setProcesses(processes);
}

void ProcessesTask::setQueuesLists(const QueuesLists &queues) {
  auto queue1 = static_cast<std::size_t>(ui->spinBoxQueue1->value());
  auto queue2 = static_cast<std::size_t>(ui->spinBoxQueue2->value());

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
      {StrategyType::SJN, "Стратегия: SJN"_qs},
      {StrategyType::SRT, "Стратегия: SRT"_qs},
      {StrategyType::WINDOWS, "Стратегия: Windows NT"_qs},
      {StrategyType::UNIX, "Стратегия: Unix"_qs},
      {StrategyType::LINUXO1, "Стратегия: Linux O(1)"}};

  label->setText(strategyMap[type]);
}

void ProcessesTask::setCompletedTaskCount(std::size_t count,
                                          std::size_t total) {
  ui->completeTaskLabel->setText(
      "Обработано заявок: %1 из %2"_qs.arg(count).arg(total));
}

void ProcessesTask::pushToQueue(QLineEdit *lineEdit, QSpinBox *spinBox) {
  if (lineEdit->text().isEmpty()) {
    return;
  }
  auto pid = lineEdit->text().toInt();
  auto queue = static_cast<std::size_t>(spinBox->value());

  try {
    _model.state = collectState();
    auto tmp = _model.state;
    tmp = changeProcessState(tmp, pid, ProcState::ACTIVE);
    _model.state = ProcessesManagement::pushToQueue(tmp, queue, pid);
    refresh();

    lineEdit->clear();
  } catch (const OperationException &ex) {
    if (ex.what() == "NO_SUCH_PROCESS"s) {
      warning("Процесс с таким PID не существует");
    } else if (ex.what() == "ALREADY_IN_QUEUE"s) {
      warning("Процесс с таким PID уже добавлен в одну из очередей");
    } else {
      throw;
    }
  } catch (const TypeException &ex) {
    if (ex.what() == "INVALID_PRIORITY"s &&
        _model.task.strategy()->type() == StrategyType::WINDOWS) {
      warning(
          "Процесс нельзя добавить в очередь с приоритетом меньше базового");
    } else {
      throw;
    }
  } catch (const std::exception &ex) {
    warning("Неизвестная ошибка: "s + ex.what());
  }
}

void ProcessesTask::popFromQueue(std::size_t queue, QLineEdit *lineEdit) {
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
      warning("Очередь пуста");
    } else {
      throw;
    }
  } catch (const std::exception &ex) {
    warning("Неизвестная ошибка: "s + ex.what());
  }
}

void ProcessesTask::warning(const std::string &message) {
  QMessageBox::warning(this, "Ошибка", QString::fromStdString(message));
}
