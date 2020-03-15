#include <cstddef>
#include <exception>
#include <map>
#include <string>
#include <vector>

#include <QDebug>
#include <QFlags>
#include <QIntValidator>
#include <QLineEdit>
#include <QListWidgetItem>
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
#include <qtutils/fontscale.h>

#include <dialogs/createprocessdialog.h>
#include <menus/processmenu.h>
#include <processestablewidget.h>
#include <reorderablelistwidget.h>

#include "models.h"

#include "processestask.h"
#include "ui_processestask.h"

using namespace std::string_literals;
using namespace ProcessesManagement;
using namespace QtUtils::Literals;

static const QString ACTIONS_TEMPL("Действия пользователя:%1");

static const QString ACTIONS_UNAVAILABLE("\n<нет информации>");

static const QString NO_ACTIONS("\n<нет>");

ProcessesTask::ProcessesTask(Models::ProcessesModel model, QWidget *parent)
    : QWidget(parent), _model(model), currentRequest(model.task.completed()),

      currentActions(""), actions(model.task.completed(), ""),
      states(model.task.requests().size()), ui(new Ui::ProcessesTask) {
  ui->setupUi(this);

  auto font = QApplication::font();
  font.setPointSizeF(font.pointSizeF() * FONT_SCALE_FACTOR);
  ui->labelRequestDescr->setFont(font);

  ui->processesTable->setContextMenuPolicy(Qt::CustomContextMenu);

  ui->lineEditQueue1Push->setValidator(new QIntValidator(0, 255));
  ui->lineEditQueue2Push->setValidator(new QIntValidator(0, 255));

  setupSignals();

  auto state = ProcessesState::initial();
  auto strategy = _model.task.strategy();
  for (size_t i = 0; i < _model.task.completed(); ++i) {
    auto request = _model.task.requests().at(i);
    state = strategy->processRequest(request, state);
    states[i] = state;
  }

  for (size_t i = 0; i < _model.task.completed(); ++i) {
    QString action;
    if (i < _model.task.actions().size()) {
      actions[i] = QString::fromStdString(_model.task.actions()[i]);
    } else {
      actions[i] = ACTIONS_UNAVAILABLE;
    }
  }

  updateHistoryView(_model.task, actions);
  refresh();
}

Utils::Task ProcessesTask::task() const {
  std::vector<std::string> stdStrActions(actions.size());
  for (size_t i = 0; i < actions.size(); ++i) {
    stdStrActions[i] = actions[i].toStdString();
  }

  const auto &task = _model.task;
  return Utils::ProcessesTask::create(task.strategy(),
                                      task.completed(),
                                      task.fails(),
                                      task.state(),
                                      task.requests(),
                                      stdStrActions);
}

ProcessesTask::~ProcessesTask() { delete ui; }

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
    currentActions +=
        "\nСоздан новый процесс (PID=%1, PPID=%2)"_qs.arg(process->pid())
            .arg(process->ppid());
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
    currentActions += "\nУдаление из списка процесса PID=%1"_qs.arg(pid);
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
    currentActions += "\nПереключение на процесс PID=%1"_qs.arg(pid);
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
    currentActions +=
        "\nПереключение процесса PID=%1 в состояние ожидания"_qs.arg(pid);
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
    currentActions += "\nPID=%1 готов к выполнению"_qs.arg(pid);
    refresh();
  } catch (const std::exception &ex) {
    warning("Неизвестная ошибка: "s + ex.what());
  }
}

void ProcessesTask::pushToQueue(int pid, std::size_t queue) {
  try {
    _model.state = collectState();
    auto tmp = _model.state;
    tmp = changeProcessState(tmp, pid, ProcState::ACTIVE);
    _model.state = ProcessesManagement::pushToQueue(tmp, queue, pid);
    currentActions +=
        "\nДобавление процесса PID=%1 в очередь #%2"_qs.arg(pid).arg(queue);
    refresh();
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

int32_t ProcessesTask::popFromQueue(std::size_t queue) {
  int32_t pid = -1;

  try {
    _model.state = collectState();
    const auto &q = _model.state.queues[queue];
    if (!q.empty()) {
      pid = q.front();
    }
    _model.state = ProcessesManagement::popFromQueue(_model.state, queue);
    currentActions += "\nЧтение из очереди #%1, PID=%2"_qs.arg(queue).arg(pid);

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

  return pid;
}

void ProcessesTask::updateCurrentRequest(int index) {
  if (index == -1) {
    return;
  }
  auto indexu = static_cast<std::size_t>(index);
  currentRequest = indexu;
  if (indexu == _model.task.completed() && !_model.task.done()) {
    unlockUi();
  } else {
    lockUi();
  }
  refresh();

  qDebug() << "currentRequest: " << currentRequest;
}

void ProcessesTask::updateQueuesLists(int) { refresh(); }

void ProcessesTask::refresh() {
  auto request = currentRequest == _model.task.requests().size()
                     ? _model.task.requests().back()
                     : _model.task.requests().at(currentRequest);
  auto state = currentRequest == _model.task.completed()
                   ? _model.state
                   : states[currentRequest];
  auto strategy = _model.task.strategy();

  updateMainView(state, request);
  updateStrategyView(strategy->type());
  updateStatsView(_model.task.completed(),
                  _model.task.requests().size(),
                  _model.task.fails());
  updateCurrentActionsView(_model.task, currentActions);
}

void ProcessesTask::nextRequest() {
  auto state = updateTimer(collectState());

  if (auto [ok, task] = _model.task.next(state); ok) {
    _model.task = task;
    _model.state = _model.task.state();
    states[currentRequest] = _model.state;

    if (_model.task.done()) {
      QMessageBox::information(
          this, "Внимание", "Вы успешно выполнили данное задание");
      lockUi();
    }
    actions.push_back(currentActions);
    currentActions = "";
    updateHistoryView(_model.task, actions);
  } else {
    _model.task = task;
    warning("Заявка обработана неверно");
  }

  refresh();

  qDebug() << "currentRequest: " << currentRequest;
}

void ProcessesTask::setupSignals() {
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
          &ProcessesTask::updateQueuesLists);
  connect(ui->spinBoxQueue2,
          QOverload<int>::of(&QSpinBox::valueChanged),
          this,
          &ProcessesTask::updateQueuesLists);
  auto push1Handler = [=]() {
    pushToQueueHandler(ui->lineEditQueue1Push, ui->spinBoxQueue1);
  };
  auto push2Handler = [=]() {
    pushToQueueHandler(ui->lineEditQueue2Push, ui->spinBoxQueue2);
  };
  connect(ui->pushButtonQueue1Push, &QPushButton::clicked, this, push1Handler);
  connect(
      ui->lineEditQueue1Push, &QLineEdit::returnPressed, this, push1Handler);
  connect(ui->pushButtonQueue2Push, &QPushButton::clicked, this, push2Handler);
  connect(
      ui->lineEditQueue2Push, &QLineEdit::returnPressed, this, push2Handler);
  connect(ui->pushButtonQueue1Pop, &QPushButton::clicked, this, [=]() {
    popFromQueueHandler(ui->lineEditQueue1Pop, ui->spinBoxQueue1);
  });
  connect(ui->pushButtonQueue2Pop, &QPushButton::clicked, this, [=]() {
    popFromQueueHandler(ui->lineEditQueue2Pop, ui->spinBoxQueue2);
  });
  connect(ui->processesTable,
          &ProcessesTableWidget::customContextMenuRequested,
          this,
          &ProcessesTask::provideContextMenu);
  connect(
      ui->listQueue1, &ReorderableListWidget::itemsOrderChanged, this, [=]() {
        currentActions.clear();
        _model.state = collectState();
        refresh();
      });
  connect(ui->historyList,
          &QListWidget::currentRowChanged,
          this,
          &ProcessesTask::updateCurrentRequest);
}

void ProcessesTask::lockUi() {
  ui->resetState->setDisabled(true);
  ui->acceptRequest->setDisabled(true);
  ui->pushButtonQueue1Pop->setDisabled(true);
  ui->pushButtonQueue2Pop->setDisabled(true);
  ui->pushButtonQueue1Push->setDisabled(true);
  ui->pushButtonQueue2Push->setDisabled(true);
  ui->lineEditQueue1Push->setDisabled(true);
  ui->lineEditQueue2Push->setDisabled(true);
  ui->processesTable->setContextMenuPolicy(Qt::NoContextMenu);
  ui->listQueue1->setDragEnabled(false);
}

void ProcessesTask::unlockUi() {
  ui->resetState->setEnabled(true);
  ui->acceptRequest->setEnabled(true);
  ui->pushButtonQueue1Pop->setEnabled(true);
  ui->pushButtonQueue2Pop->setEnabled(true);
  ui->pushButtonQueue1Push->setEnabled(true);
  ui->pushButtonQueue2Push->setEnabled(true);
  ui->lineEditQueue1Push->setEnabled(true);
  ui->lineEditQueue2Push->setEnabled(true);
  ui->processesTable->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->listQueue1->setDragEnabled(true);
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

void ProcessesTask::provideContextMenu(const QPoint &pos) {
  qDebug() << "ContextMenu";

  auto *processes = ui->processesTable;

  auto globalPos = processes->mapToGlobal(pos);
  auto *item = processes->itemAt(pos);
  auto row = item ? item->row() : -1;
  _model.state = collectState();
  auto process =
      row != -1
          ? tl::optional<Process>{_model.state.processes.at(mapRowToIndex(row))}
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

void ProcessesTask::pushToQueueHandler(QLineEdit *lineEdit, QSpinBox *spinBox) {
  if (lineEdit->text().isEmpty()) {
    return;
  }
  auto pid = lineEdit->text().toInt();
  auto queue = static_cast<std::size_t>(spinBox->value());

  pushToQueue(pid, queue);

  lineEdit->clear();
}

void ProcessesTask::popFromQueueHandler(QLineEdit *lineEdit,
                                        QSpinBox *spinBox) {
  auto queue = static_cast<std::size_t>(spinBox->value());
  auto pid = popFromQueue(queue);
  if (pid != -1) {
    lineEdit->setText("%1"_qs.arg(pid));
  }
}

std::size_t ProcessesTask::mapRowToIndex(int row) {
  auto pid = ui->processesTable->item(row, 0)->text().toInt();

  return *getIndexByPid(_model.state.processes, pid);
}

void ProcessesTask::updateMainView(ProcessesState state, Request request) {
  auto [processes, queues] = state;
  setProcessesList(processes);
  setQueuesLists(queues);
  setRequest(request);
}

void ProcessesTask::updateStatsView(std::size_t count,
                                    std::size_t total,
                                    uint32_t fails) {
  ui->statsLabel->setText(
      "Обработано заявок: %1 из %2; ошибок: %3"_qs.arg(count).arg(total).arg(
          fails));
}

void ProcessesTask::updateStrategyView(StrategyType type) {
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

void ProcessesTask::updateHistoryView(Utils::ProcessesTask task,
                                      const std::vector<QString> &actions) {
  ui->historyList->clear();
  auto templ = QString("Заявка #%1");
  for (size_t i = 0; i < task.completed(); ++i) {
    auto *item = new QListWidgetItem();
    item->setText(templ.arg(i + 1));
    item->setToolTip(
        ACTIONS_TEMPL.arg(actions[i].isEmpty() ? NO_ACTIONS : actions[i]));
    ui->historyList->addItem(item);
  }
  if (!task.done()) {
    ui->historyList->addItem(templ.arg(ui->historyList->count() + 1));
  }
  ui->historyList->setCurrentRow(ui->historyList->count() - 1);
}

void ProcessesTask::updateCurrentActionsView(Utils::ProcessesTask task,
                                             const QString actions) {
  if (auto *item = ui->historyList->item(ui->historyList->count() - 1);
      item && !task.done()) {
    item->setToolTip(
        ACTIONS_TEMPL.arg(actions.isEmpty() ? NO_ACTIONS : actions));
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

void ProcessesTask::warning(const std::string &message) {
  QMessageBox::warning(this, "Ошибка", QString::fromStdString(message));
}
