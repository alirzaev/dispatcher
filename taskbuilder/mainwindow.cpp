#include <fstream>
#include <utility>

#include <QDebug>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QString>

#include <utils/io.h>
#include <utils/tasks.h>

#include <qtutils/fileio.h>
#include <qtutils/literals.h>

#include "aboutwindow.h"
#include "memorytaskbuilder.h"
#include "menus/requestitemmenu.h"
#include "menus/taskitemmenu.h"
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

  connect(ui->actionQuit, &QAction::triggered, this, &QMainWindow::close);
  connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showHelp);

  connect(ui->tasksList,
          &QListWidget::customContextMenuRequested,
          this,
          &MainWindow::provideContextMenu);

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
    qCritical() << ex.what();
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
          dynamic_cast<AbstractTaskBuilder *>(ui->currentTaskWidget->widget(i));
      auto task = widget->task();
      tasks.push_back(task);
    }

    Utils::saveTasks(tasks, file);
  } catch (const std::exception &ex) {
    qCritical() << ex.what();
    QMessageBox::warning(this, "Ошибка", "Невозможно сохранить задания");
  }
}

void MainWindow::loadTasks(const std::vector<Utils::Task> &tasks) {
  removeTasks();

  for (const auto &task : tasks) {
    auto widget = task.match(
        [this](const Utils::MemoryTask &task) {
          AbstractTaskBuilder *widget = new MemoryTaskBuilder(task, this);
          return widget;
        },
        [this](const Utils::ProcessesTask &task) {
          AbstractTaskBuilder *widget = new ProcessesTaskBuilder(task, this);
          return widget;
        });

    attachTask(widget);
  }
}

void MainWindow::createTask(QAction *action) {
  AbstractTaskBuilder *widget = nullptr;

  if (action == ui->actionFA) {
    Utils::Task task = Utils::MemoryTask::create(
        MemoryManagement::FirstAppropriateStrategy::create(),
        0,
        MemoryManagement::MemoryState::initial(),
        {});
    widget = new MemoryTaskBuilder(task, this);
  } else if (action == ui->actionLA) {
    Utils::Task task = Utils::MemoryTask::create(
        MemoryManagement::LeastAppropriateStrategy::create(),
        0,
        MemoryManagement::MemoryState::initial(),
        {});
    widget = new MemoryTaskBuilder(task, this);
  } else if (action == ui->actionMA) {
    Utils::Task task = Utils::MemoryTask::create(
        MemoryManagement::MostAppropriateStrategy::create(),
        0,
        MemoryManagement::MemoryState::initial(),
        {});
    widget = new MemoryTaskBuilder(task, this);
  } else if (action == ui->actionSJN) {
    Utils::Task task = Utils::ProcessesTask::create(
        ProcessesManagement::SjnStrategy::create(),
        0,
        ProcessesManagement::ProcessesState::initial(),
        {});
    widget = new ProcessesTaskBuilder(task, this);
  } else if (action == ui->actionSRT) {
    Utils::Task task = Utils::ProcessesTask::create(
        ProcessesManagement::SrtStrategy::create(),
        0,
        ProcessesManagement::ProcessesState::initial(),
        {});
    widget = new ProcessesTaskBuilder(task, this);
  } else if (action == ui->actionFCFS) {
    Utils::Task task = Utils::ProcessesTask::create(
        ProcessesManagement::FcfsStrategy::create(),
        0,
        ProcessesManagement::ProcessesState::initial(),
        {});
    widget = new ProcessesTaskBuilder(task, this);
  } else if (action == ui->actionRoundRobin) {
    Utils::Task task = Utils::ProcessesTask::create(
        ProcessesManagement::RoundRobinStrategy::create(),
        0,
        ProcessesManagement::ProcessesState::initial(),
        {});
    widget = new ProcessesTaskBuilder(task, this);
  } else if (action == ui->actionUNIX) {
    Utils::Task task = Utils::ProcessesTask::create(
        ProcessesManagement::UnixStrategy::create(),
        0,
        ProcessesManagement::ProcessesState::initial(),
        {});
    widget = new ProcessesTaskBuilder(task, this);
  } else if (action == ui->actionWinNT) {
    Utils::Task task = Utils::ProcessesTask::create(
        ProcessesManagement::WinNtStrategy::create(),
        0,
        ProcessesManagement::ProcessesState::initial(),
        {});
    widget = new ProcessesTaskBuilder(task, this);
  } else if (action == ui->actionLinux_O_1) {
    Utils::Task task = Utils::ProcessesTask::create(
        ProcessesManagement::LinuxO1Strategy::create(),
        0,
        ProcessesManagement::ProcessesState::initial(),
        {});
    widget = new ProcessesTaskBuilder(task, this);
  }

  attachTask(widget);
}

void MainWindow::removeTasks() {
  auto *stack = ui->currentTaskWidget;
  while (stack->count() > 0) {
    auto *widget = stack->widget(0);
    stack->removeWidget(widget);
    delete widget;
  }

  auto *list = ui->tasksList;
  list->clear();
}

void MainWindow::attachTask(AbstractTaskBuilder *taskWidget) {
  connect(taskWidget,
          &AbstractTaskBuilder::historyStateChanged,
          this,
          &MainWindow::updateMenuEditState);
  connect(taskWidget,
          &AbstractTaskBuilder::historyStateChanged,
          this,
          [this]() {
            this->updateTaskPreview(ui->currentTaskWidget->currentIndex());
          });

  ui->currentTaskWidget->addWidget(taskWidget);
  ui->tasksList->addItem(new QListWidgetItem());
  ui->tasksList->setEnabled(true);

  updateTaskPreview(ui->currentTaskWidget->count() - 1);
}

void MainWindow::showHelp() {
  auto window = AboutWindow(this);
  window.exec();
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

void MainWindow::updateTaskPreview(int index) {

  auto *taskWidget =
      dynamic_cast<AbstractTaskBuilder *>(ui->currentTaskWidget->widget(index));

  QString taskType = taskWidget->task().match(
      [](const Utils::MemoryTask &) { return "Диспетчеризация памяти"; },
      [](const Utils::ProcessesTask &) { return "Диспетчеризация процессов"; });
  auto requestsCount = taskWidget->task().match(
      [](const auto &task) { return task.requests().size(); });
  auto strategy = taskWidget->strategy();

  auto *listItem = ui->tasksList->item(index);
  listItem->setText("Задание #%1"_qs.arg(index + 1));
  listItem->setToolTip(
      "%1\nСтратегия: %2. Заявок: %3"_qs.arg(taskType).arg(strategy).arg(
          requestsCount));
}

void MainWindow::processContextMenuAction(const QString &action,
                                          int taskIndex) {
  auto *stack = ui->currentTaskWidget;

  if (action == RequestItemMenu::TO_TOP) {
    auto *taskWidget = stack->widget(taskIndex);
    stack->removeWidget(taskWidget);
    stack->insertWidget(0, taskWidget);
  } else if (action == RequestItemMenu::TO_BOTTOM) {
    auto *taskWidget = stack->widget(taskIndex);
    stack->removeWidget(taskWidget);
    stack->addWidget(taskWidget);
  } else if (action == RequestItemMenu::MOVE_UP && taskIndex > 0) {
    auto *taskWidget = stack->widget(taskIndex);
    stack->removeWidget(taskWidget);
    stack->insertWidget(taskIndex - 1, taskWidget);
  } else if (action == RequestItemMenu::MOVE_DOWN &&
             taskIndex < stack->count() - 1) {
    auto *taskWidget = stack->widget(taskIndex);
    stack->removeWidget(taskWidget);
    stack->insertWidget(taskIndex + 1, taskWidget);
  } else if (action == RequestItemMenu::DELETE) {
    auto answer = QMessageBox::question(
        this,
        "Удаление задания",
        "Вы действительно хотите удалить задание? Это действие необратимо.");
    if (answer == QMessageBox::Yes) {
      auto *taskWidget = stack->widget(taskIndex);
      stack->removeWidget(taskWidget);
      delete taskWidget;

      auto *listItem = ui->tasksList->takeItem(taskIndex);
      delete listItem;
    }
  }

  for (int i = 0; i < ui->tasksList->count(); ++i) {
    updateTaskPreview(i);
  }
}

void MainWindow::provideContextMenu(const QPoint &pos) {
  auto globalPos = ui->tasksList->mapToGlobal(pos);
  auto row = ui->tasksList->indexAt(pos).row();

  TaskItemMenu menu(row != -1);

  auto action = menu.exec(globalPos);
  if (!action) {
    return;
  }

  processContextMenuAction(action->text(), row);
}
