#include <fstream>
#include <utility>

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>

#include <utils/io.h>
#include <utils/tasks.h>

#include <qtutils/fileio.h>
#include <qtutils/literals.h>

#include "memorytaskbuilder.h"
#include "processestaskbuilder.h"

#include "historynavigator.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace QtUtils::Literals;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(
      ui->actionOpenTask, &QAction::triggered, this, &MainWindow::openTasks);
  connect(
      ui->actionSaveTask, &QAction::triggered, this, &MainWindow::saveTasks);
  connect(ui->tasksList,
          &QListWidget::currentRowChanged,
          ui->currentTaskWidget,
          &QStackedWidget::setCurrentIndex);
  connect(ui->currentTaskWidget,
          &QStackedWidget::currentChanged,
          this,
          &MainWindow::updateMenuEditState);
  connect(ui->menuAddTask, &QMenu::triggered, this, &MainWindow::createTask);

  connect(ui->actionUndo, &QAction::triggered, this, &MainWindow::undoAction);
  connect(ui->actionRedo, &QAction::triggered, this, &MainWindow::redoAction);

  ui->actionUndo->setDisabled(true);
  ui->actionRedo->setDisabled(true);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::openTasks() {
  auto fileName = QFileDialog::getOpenFileName(
      this, "Открыть файл задания", "", "JSON (*.json)");
  if (fileName.isEmpty()) {
    return;
  }

  try {
    std::ifstream file = QtUtils::FileIO::openStdIfstream(fileName);
    if (!file) {
      QMessageBox::warning(this, "Ошибка", "Невозможно открыть файл задания");
      return;
    }

    auto tasks = Utils::loadTasks(file);
    loadTasks(tasks);
  } catch (const std::exception &ex) {
    qDebug() << ex.what();
    QMessageBox::warning(
        this, "Ошибка", "Невозможно загрузить задания: файл поврежден");
  }
}

void MainWindow::saveTasks() {
  auto fileName = QFileDialog::getSaveFileName(
      this, "Сохранить задание в файл", "", "JSON (*.json)");
  if (fileName.isEmpty()) {
    return;
  }

  if (!fileName.endsWith(".json")) {
    fileName.append(".json");
  }

  try {
    std::ofstream file = QtUtils::FileIO::openStdOfstream(fileName);

    std::vector<Utils::Task> tasks;

    for (int i = 0; i < ui->currentTaskWidget->count(); ++i) {
      auto *widget =
          dynamic_cast<TaskGetter *>(ui->currentTaskWidget->widget(i));
      auto task = widget->task();
      tasks.push_back(task);
    }

    Utils::saveTasks(tasks, file);
  } catch (const std::exception &ex) {
    qDebug() << ex.what();
    QMessageBox::warning(this, "Ошибка", "Невозможно сохранить задания");
  }
}

void MainWindow::loadTasks(const std::vector<Utils::Task> &tasks) {
  auto *stack = ui->currentTaskWidget;
  while (stack->count() > 0) {
    auto *widget = stack->widget(0);
    stack->removeWidget(widget);
    delete widget;
  }

  auto *list = ui->tasksList;
  list->setEnabled(true);
  list->clear();

  for (const auto &task : tasks) {
    auto [widget, label] = task.match(
        [this](const Utils::MemoryTask &task) {
          AbstractTaskBuilder *widget = new MemoryTaskBuilder(task, this);
          return std::pair{widget, "Диспетчеризация памяти"_qs};
        },
        [this](const Utils::ProcessesTask &task) {
          AbstractTaskBuilder *widget = new ProcessesTaskBuilder(task, this);
          return std::pair{widget, "Диспетчеризация процессов"_qs};
        });
    connect(widget,
            &AbstractTaskBuilder::historyStateChanged,
            this,
            &MainWindow::updateMenuEditState);

    stack->addWidget(dynamic_cast<QWidget *>(widget));
    list->addItem(label);
  }
}

void MainWindow::createTask(QAction *action) {
  QWidget *widget = nullptr;
  QString label;

  if (action == ui->actionFA) {
    Utils::Task task = Utils::MemoryTask::create(
        MemoryManagement::FirstAppropriateStrategy::create(),
        0,
        MemoryManagement::MemoryState::initial(),
        {});
    widget = new MemoryTaskBuilder(task, this);
    label = "Диспетчеризация памяти";
  } else if (action == ui->actionLA) {
    Utils::Task task = Utils::MemoryTask::create(
        MemoryManagement::LeastAppropriateStrategy::create(),
        0,
        MemoryManagement::MemoryState::initial(),
        {});
    widget = new MemoryTaskBuilder(task, this);
    label = "Диспетчеризация памяти";
  } else if (action == ui->actionMA) {
    Utils::Task task = Utils::MemoryTask::create(
        MemoryManagement::MostAppropriateStrategy::create(),
        0,
        MemoryManagement::MemoryState::initial(),
        {});
    widget = new MemoryTaskBuilder(task, this);
    label = "Диспетчеризация памяти";
  } else if (action == ui->actionSJN) {
    Utils::Task task = Utils::ProcessesTask::create(
        ProcessesManagement::SjnStrategy::create(),
        0,
        ProcessesManagement::ProcessesState::initial(),
        {});
    widget = new ProcessesTaskBuilder(task, this);
    label = "Диспетчеризация процессов";
  } else if (action == ui->actionSRT) {
    Utils::Task task = Utils::ProcessesTask::create(
        ProcessesManagement::SrtStrategy::create(),
        0,
        ProcessesManagement::ProcessesState::initial(),
        {});
    widget = new ProcessesTaskBuilder(task, this);
    label = "Диспетчеризация процессов";
  } else if (action == ui->actionFCFS) {
    Utils::Task task = Utils::ProcessesTask::create(
        ProcessesManagement::FcfsStrategy::create(),
        0,
        ProcessesManagement::ProcessesState::initial(),
        {});
    widget = new ProcessesTaskBuilder(task, this);
    label = "Диспетчеризация процессов";
  } else if (action == ui->actionRoundRobin) {
    Utils::Task task = Utils::ProcessesTask::create(
        ProcessesManagement::RoundRobinStrategy::create(),
        0,
        ProcessesManagement::ProcessesState::initial(),
        {});
    widget = new ProcessesTaskBuilder(task, this);
    label = "Диспетчеризация процессов";
  } else if (action == ui->actionUNIX) {
    Utils::Task task = Utils::ProcessesTask::create(
        ProcessesManagement::UnixStrategy::create(),
        0,
        ProcessesManagement::ProcessesState::initial(),
        {});
    widget = new ProcessesTaskBuilder(task, this);
    label = "Диспетчеризация процессов";
  } else if (action == ui->actionWinNT) {
    Utils::Task task = Utils::ProcessesTask::create(
        ProcessesManagement::WinNtStrategy::create(),
        0,
        ProcessesManagement::ProcessesState::initial(),
        {});
    widget = new ProcessesTaskBuilder(task, this);
    label = "Диспетчеризация процессов";
  } else if (action == ui->actionLinux_O_1) {
    Utils::Task task = Utils::ProcessesTask::create(
        ProcessesManagement::LinuxO1Strategy::create(),
        0,
        ProcessesManagement::ProcessesState::initial(),
        {});
    widget = new ProcessesTaskBuilder(task, this);
    label = "Диспетчеризация процессов";
  }

  connect(dynamic_cast<AbstractTaskBuilder *>(widget),
          &AbstractTaskBuilder::historyStateChanged,
          this,
          &MainWindow::updateMenuEditState);

  ui->currentTaskWidget->addWidget(widget);
  ui->tasksList->addItem(label);
  ui->tasksList->setEnabled(true);
}

void MainWindow::closeEvent(QCloseEvent *event) {
  auto answer = QMessageBox::question(
      this, "Завершение работы", "Вы действительно хотите выйти?");
  if (answer == QMessageBox::Yes) {
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::undoAction() {
  if (auto *navigator = dynamic_cast<HistoryNavigator *>(
          ui->currentTaskWidget->currentWidget());
      navigator) {
    navigator->backward();
  }
}

void MainWindow::redoAction() {
  if (auto *navigator = dynamic_cast<HistoryNavigator *>(
          ui->currentTaskWidget->currentWidget());
      navigator) {
    navigator->forward();
  }
}

void MainWindow::updateMenuEditState() {
  auto *navigator =
      dynamic_cast<HistoryNavigator *>(ui->currentTaskWidget->currentWidget());
  ui->actionUndo->setDisabled(!navigator || !navigator->hasPrevious());
  ui->actionRedo->setDisabled(!navigator || !navigator->hasNext());
}
