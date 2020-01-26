#include <fstream>

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

#include <utils/io.h>
#include <utils/tasks.h>

#include <qtutils/fileio.h>

#include "memorytaskbuilder.h"
#include "processestaskbuilder.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

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
  int count = stack->count();
  for (int i = 0; i < count; ++i) {
    auto *widget = stack->widget(i);
    stack->removeWidget(widget);
    delete widget;
  }
  auto *list = ui->tasksList;
  list->setEnabled(true);
  list->clear();

  for (const auto &task : tasks) {
    task.match(
        [stack, list, this](const Utils::MemoryTask &task) {
          auto *taskWidget = new MemoryTaskBuilder(task, this);
          stack->addWidget(taskWidget);
          list->addItem("Диспетчеризация памяти");
        },
        [stack, list, this](const Utils::ProcessesTask &task) {
          auto *taskWidget = new ProcessesTaskBuilder(task, this);
          stack->addWidget(taskWidget);
          list->addItem("Диспетчеризация процессов");
        });
  }
}
