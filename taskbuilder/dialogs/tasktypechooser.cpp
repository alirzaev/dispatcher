#include <map>
#include <vector>

#include <QComboBox>
#include <QDebug>

#include <algo/memory/strategies.h>
#include <algo/processes/strategies.h>

#include "tasktypechooser.h"
#include "ui_tasktypechooser.h"

static std::vector<MemoryManagement::StrategyPtr> memoryStrategies = {
    MemoryManagement::FirstAppropriateStrategy::create(),
    MemoryManagement::MostAppropriateStrategy::create(),
    MemoryManagement::LeastAppropriateStrategy::create()};

static std::vector<ProcessesManagement::StrategyPtr> processesStrategies = {
    ProcessesManagement::FcfsStrategy::create(),
    ProcessesManagement::LinuxO1Strategy::create(),
    ProcessesManagement::RoundRobinStrategy::create(),
    ProcessesManagement::SjnStrategy::create(),
    ProcessesManagement::SrtStrategy::create(),
    ProcessesManagement::UnixStrategy::create(),
    ProcessesManagement::WinNtStrategy::create()};

TaskTypeChooser::TaskTypeChooser(QWidget *parent)
    : QDialog(parent), ui(new Ui::TaskTypeChooser) {
  ui->setupUi(this);

  ui->taskTypeChooser->addItem("Диспетчеризация памяти", 0);
  ui->taskTypeChooser->addItem("Диспетчеризация процессов", 1);

  connect(ui->taskTypeChooser,
          QOverload<int>::of(&QComboBox::activated),
          this,
          &TaskTypeChooser::loadStrategies,
          Qt::QueuedConnection);
  connect(ui->buttonBox,
          &QDialogButtonBox::accepted,
          this,
          &TaskTypeChooser::tryAccept);
  connect(ui->buttonBox,
          &QDialogButtonBox::rejected,
          this,
          &TaskTypeChooser::reject);

  ui->taskTypeChooser->setCurrentIndex(-1);
}

TaskTypeChooser::~TaskTypeChooser() { delete ui; }

void TaskTypeChooser::loadStrategies(int taskType) {
  this->taskType = taskType;

  auto *box = ui->strategyChooser;
  box->clear();

  if (taskType == 0) {
    for (auto ptr : memoryStrategies) {
      auto name = QString::fromStdString(ptr->toString());
      box->addItem(name);
    }
  } else if (taskType == 1) {
    for (auto ptr : processesStrategies) {
      auto name = QString::fromStdString(ptr->toString());
      box->addItem(name);
    }
  }
}

void TaskTypeChooser::tryAccept() {
  auto strategys = ui->strategyChooser->currentText();
  if (taskType != -1 && !strategys.isNull() && !strategys.isEmpty()) {
    this->data = {taskType, strategy};
    this->accept();
  }
}
