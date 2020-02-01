#include <map>

#include <QDebug>

#include <utils/tasks.h>

#include <qtutils/literals.h>

#include <dialogs/createprocessdialog.h>

#include "dialogs/pidinputdialog.h"
#include "dialogs/terminateiodialog.h"
#include "menus/processtaskaddrequestmenu.h"
#include "menus/requestitemmenu.h"

#include "processestaskbuilder.h"
#include "ui_processestaskbuilder.h"

using namespace ProcessesManagement;
using namespace QtUtils::Literals;

static std::map<StrategyType, QString> strategyMap = {
    {StrategyType::ROUNDROBIN, "Round Robin"_qs},
    {StrategyType::FCFS, "FCFS"_qs},
    {StrategyType::SJN, "SJN"_qs},
    {StrategyType::SRT, "SRT"_qs},
    {StrategyType::WINDOWS, "Windows NT"_qs},
    {StrategyType::UNIX, "Unix"_qs},
    {StrategyType::LINUXO1, "Linux O(1)"_qs}};

ProcessesTaskBuilder::ProcessesTaskBuilder(const Utils::Task &task,
                                           QWidget *parent)
    : AbstractTaskBuilder(parent), HistoryNavigator(task),
      _task(task.get<Utils::ProcessesTask>()), currentRequest(-1),
      ui(new Ui::ProcessesTaskBuilder) {
  ui->setupUi(this);

  connect(ui->requestsList,
          &QListWidget::currentRowChanged,
          this,
          &ProcessesTaskBuilder::currentRequestChanged);
  connect(ui->requestsList,
          &QListWidget::customContextMenuRequested,
          this,
          &ProcessesTaskBuilder::provideContextMenu);
  connect(ui->spinBoxQueue1,
          QOverload<int>::of(&QSpinBox::valueChanged),
          this,
          &ProcessesTaskBuilder::queuesListsChanged);
  connect(ui->spinBoxQueue2,
          QOverload<int>::of(&QSpinBox::valueChanged),
          this,
          &ProcessesTaskBuilder::queuesListsChanged);

  loadTask(_task);
  selectCurrentRequest(0);
}

ProcessesTaskBuilder::~ProcessesTaskBuilder() { delete ui; }

void ProcessesTaskBuilder::loadTask(const Utils::ProcessesTask &task) {
  auto state = ProcessesState::initial();

  states.clear();
  for (const auto &req : task.requests()) {
    state = task.strategy()->processRequest(req, state);
    states.push_back(state);
  }
  setRequestsList(task.requests());
  setStrategy(task.strategy()->type());
}

void ProcessesTaskBuilder::queuesListsChanged(int) {
  auto state =
      currentRequest == -1 ? ProcessesState::initial() : states[currentRequest];
  setQueuesLists(state.queues);
}

void ProcessesTaskBuilder::currentRequestChanged(int index) {
  this->currentRequest = index;
  auto indexu = static_cast<std::size_t>(index);
  if (index < 0 || indexu > _task.requests().size()) {
    return;
  }

  auto state = states[index];
  auto request = _task.requests().at(indexu);
  updateTaskView(state, request);
}

void ProcessesTaskBuilder::processContextMenuAction(const QString &action,
                                                    int requestIndex) {
  auto requests = _task.requests();
  bool changed = false;

  if (requestIndex != -1) {
    auto requestIndexu = static_cast<std::size_t>(requestIndex);
    auto request = requests.at(requestIndexu);

    if (action == RequestItemMenu::TO_TOP) {
      requests.erase(requests.begin() + requestIndex);
      requests.insert(requests.begin(), request);
      requestIndex = 0;
      requestIndexu = 0u;
      changed = true;
    } else if (action == RequestItemMenu::TO_BOTTOM) {
      requests.erase(requests.begin() + requestIndex);
      requests.push_back(request);
      requestIndex = static_cast<int>(requests.size() - 1);
      requestIndexu = requests.size() - 1;
      changed = true;
    } else if (action == RequestItemMenu::MOVE_UP && requestIndex > 0) {
      std::swap(requests.at(requestIndexu - 1), requests.at(requestIndexu));
      requestIndex--;
      requestIndexu--;
      changed = true;
    } else if (action == RequestItemMenu::MOVE_DOWN &&
               requestIndexu < requests.size() - 1) {
      std::swap(requests.at(requestIndexu + 1), requests.at(requestIndexu));
      requestIndex++;
      requestIndexu++;
      changed = true;
    } else if (action == RequestItemMenu::DELETE) {
      requests.erase(requests.begin() + requestIndex);
      requestIndex = requestIndex > 0 ? requestIndex - 1 : 0;
      requestIndexu = requestIndexu > 0 ? requestIndexu - 1 : 0;
      changed = true;
    }
  }

  if (auto request = processAddRequestMenuAction(action); request.has_value()) {
    requests.push_back(*request);
    changed = true;
  }

  _task = Utils::ProcessesTask::create(
      _task.strategy(), 0, ProcessesState::initial(), requests);
  loadTask(_task);

  if (changed) {
    push(_task);
    emit historyStateChanged();
  }

  if (requestIndex == -1) {
    selectCurrentRequest(static_cast<int>(requests.size() - 1));
  } else {
    if (requests.empty()) {
      clearTaskView();
    } else {
      selectCurrentRequest(requestIndex);
    }
  }
}

tl::optional<Request>
ProcessesTaskBuilder::processAddRequestMenuAction(const QString &action) {
  bool preemptive = !(_task.strategy()->type() == StrategyType::SJN ||
                      _task.strategy()->type() == StrategyType::SRT ||
                      _task.strategy()->type() == StrategyType::FCFS);

  if (action == ProcessTaskAddRequestMenu::CREATE_PROCESS) {
    using Field = CreateProcessDialog::EditableField;

    auto flags =
        std::map<StrategyType, QFlags<Field>>{
            {StrategyType::ROUNDROBIN, {Field::Pid, Field::Ppid}},
            {StrategyType::LINUXO1, {Field::Pid, Field::Ppid}},
            {StrategyType::FCFS, {Field::Pid, Field::Ppid}},
            {StrategyType::SJN, {Field::Pid, Field::Ppid, Field::WorkTime}},
            {StrategyType::SRT, {Field::Pid, Field::Ppid, Field::WorkTime}},
            {StrategyType::UNIX, {Field::Pid, Field::Ppid, Field::Priority}},
            {StrategyType::WINDOWS,
             {Field::Pid, Field::Ppid, Field::Priority, Field::BasePriority}}}
            .at(_task.strategy()->type());

    auto process =
        CreateProcessDialog::getProcess(this, ProcessesList{}, flags, true);

    if (process) {
      return CreateProcessReq(process->pid(),
                              process->ppid(),
                              process->priority(),
                              process->basePriority(),
                              process->timer(),
                              process->workTime());
    }
  } else if (action == ProcessTaskAddRequestMenu::TERMINATE_PROCESS) {
    if (auto pid = PidInputDialog::getPid(this); pid.has_value()) {
      return TerminateProcessReq(pid.value());
    }
  } else if (action == ProcessTaskAddRequestMenu::INIT_IO) {
    if (auto pid = PidInputDialog::getPid(this); pid.has_value()) {
      return InitIO(pid.value());
    }
  } else if (action == ProcessTaskAddRequestMenu::TERMINATE_IO) {
    auto info = TerminateIoDialog::getTerminateIoInfo(
        this, _task.strategy()->type() == StrategyType::WINDOWS);

    if (info) {
      auto [pid, augment] = *info;
      return TerminateIO(pid, augment);
    }
  } else if (action == ProcessTaskAddRequestMenu::TRANSFER_CONTROL) {
    if (auto pid = PidInputDialog::getPid(this); pid.has_value()) {
      return TransferControl(pid.value());
    }
  } else if (action == ProcessTaskAddRequestMenu::TIME_QUANTUM_EXPIRED &&
             preemptive) {
    return TimeQuantumExpired();
  }

  return tl::nullopt;
}

void ProcessesTaskBuilder::selectCurrentRequest(int requestIndex) {
  ui->requestsList->setCurrentRow(requestIndex);
}

void ProcessesTaskBuilder::updateTaskView(const ProcessesState &state,
                                          const Request &request) {
  setProcessesList(state.processes);
  setQueuesLists(state.queues);
  setRequest(request);
}

void ProcessesTaskBuilder::clearTaskView() {
  auto state = ProcessesState::initial();
  setProcessesList(state.processes);
  setQueuesLists(state.queues);
  ui->labelRequestDescr->clear();
}

void ProcessesTaskBuilder::provideContextMenu(const QPoint &pos) {
  qDebug() << "ContextMenu";

  auto globalPos = ui->requestsList->mapToGlobal(pos);
  auto row = ui->requestsList->indexAt(pos).row();

  qDebug() << "ContextMenu:" << row;

  ProcessTaskAddRequestMenu menu(row != -1);

  auto action = menu.exec(globalPos);
  if (!action) {
    return;
  }

  processContextMenuAction(action->text(), row);
}

void ProcessesTaskBuilder::setProcessesList(const ProcessesList &processes) {
  ui->processesTable->setProcesses(processes);
}

void ProcessesTaskBuilder::setQueuesLists(
    const ProcessesTaskBuilder::QueuesLists &queues) {
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

void ProcessesTaskBuilder::setRequest(const Request &request) {
  auto description =
      QString::fromStdString(_task.strategy()->getRequestDescription(request));
  ui->labelRequestDescr->setText(description);
}

void ProcessesTaskBuilder::setRequestsList(
    const std::vector<Request> &requests) {
  auto *list = ui->requestsList;
  list->clear();
  for (const auto &request : requests) {
    list->addItem(request.match(
        [](const CreateProcessReq &) { return "Создать процесс"; },
        [](const TerminateProcessReq &) { return "Завершить процесс"; },
        [](const InitIO &) { return "Запрос на I/O"; },
        [](const TerminateIO &) { return "Завершить I/O"; },
        [](const TransferControl &) { return "Передать управление"; },
        [](const TimeQuantumExpired &) { return "Квант времени истек"; }));
  }
}

void ProcessesTaskBuilder::setStrategy(StrategyType type) {
  ui->strategyLabel->setText("Стратегия: %1"_qs.arg(strategyMap.at(type)));
}

void ProcessesTaskBuilder::loadTaskFromHistory(Utils::Task task) {
  _task = task.get<Utils::ProcessesTask>();
  loadTask(_task);
  clearTaskView();
  selectCurrentRequest(0);
  emit historyStateChanged();
}

QString ProcessesTaskBuilder::strategy() {
  return strategyMap.at(_task.strategy()->type());
}

Utils::Task ProcessesTaskBuilder::task() { return _task; }
