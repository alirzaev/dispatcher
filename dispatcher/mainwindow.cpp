#ifdef _WIN32
#include <filesystem>
#endif
#include <fstream>
#include <utility>
#include <vector>

#include <QAction>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QUrl>

#include <mapbox/variant.hpp>

#include <generators/memory_task.h>
#include <generators/processes_task.h>
#include <utils/io.h>
#include <utils/tasks.h>

#include <qtutils/fileio.h>

#include "models.h"
#include "taskgetter.h"

#include "aboutwindow.h"
#include "memorytask.h"
#include "processestask.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(
      ui->actionOpenTask, &QAction::triggered, this, &MainWindow::openTasks);

#ifdef RESTRICTED_MODE
  ui->actionSaveTask->setVisible(false);

  auto title = this->windowTitle();
  this->setWindowTitle(title + " (режим ограниченной функциональности)");
#else
  connect(
      ui->actionSaveTask, &QAction::triggered, this, &MainWindow::saveTasks);
#endif

  connect(ui->actionGenerateTask,
          &QAction::triggered,
          this,
          &MainWindow::createTasks);
  connect(ui->actionQuit, &QAction::triggered, this, &QMainWindow::close);
  connect(
      ui->actionOpenTmpDir, &QAction::triggered, this, &MainWindow::openTmpDir);
  connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showHelp);
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

    std::vector<Utils::Task> tasks;
    unsigned int total_count = 0;
    for (auto task : Utils::loadTasks(file)) {
      total_count++;
      bool empty =
          task.match([](const auto &t) { return t.requests().empty(); });
      if (!empty) {
        tasks.push_back(std::move(task));
      }
    }
    if (tasks.empty()) {
      QMessageBox::warning(this, "Ошибка", "Файл пуст");
      return;
    } else if (total_count > tasks.size()) {
      QMessageBox::warning(
          this, "Внимание", "Не все задания удалось загрузить");
    }

    loadTasks(tasks);
  } catch (const std::exception &ex) {
    qDebug() << ex.what();
    QMessageBox::warning(
        this, "Ошибка", "Невозможно загрузить задания: файл поврежден");
  }
}

#ifndef RESTRICTED_MODE
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

    for (int i = 0; i < ui->tabWidget->count(); ++i) {
      auto *widget = dynamic_cast<TaskGetter *>(ui->tabWidget->widget(i));
      auto task = widget->task();
      tasks.push_back(task);
    }

    Utils::saveTasks(tasks, file);
  } catch (const std::exception &ex) {
    qDebug() << ex.what();
    QMessageBox::warning(this, "Ошибка", "Невозможно сохранить задания");
  }
}
#endif

void MainWindow::loadTasks(const std::vector<Utils::Task> &tasks) {
  auto *tabs = ui->tabWidget;
  tabs->clear();

  for (const auto &task : tasks) {
    task.match(
        [tabs, this](const Utils::MemoryTask &task) {
          auto model = Models::MemoryModel{task.state(), task};

          auto *taskWidget = new MemoryTask(model, this);
          tabs->addTab(taskWidget, "Диспетчеризация памяти");
        },
        [tabs, this](const Utils::ProcessesTask &task) {
          auto model = Models::ProcessesModel{task.state(), task};

          auto *taskWidget = new ProcessesTask(model, this);
          tabs->addTab(taskWidget, "Диспетчеризация процессов");
        });
  }
}

void MainWindow::createTasks() {
  std::vector<Utils::Task> tasks = {
      Generators::MemoryTask::generate(40),
      Generators::ProcessesTask::generate(40, false),
      Generators::ProcessesTask::generate(40, true)};

  dumpTasks(tasks);
  loadTasks(tasks);
}

void MainWindow::dumpTasks(const std::vector<Utils::Task> &tasks) {
  try {
    auto tempFilePath =
        QDir::toNativeSeparators(QDir::tempPath() + "/dispatcher.json");

    qDebug() << tempFilePath;

    std::ofstream file = QtUtils::FileIO::openStdOfstream(tempFilePath);
    Utils::saveTasks(tasks, file);
  } catch (...) {
  }
}

void MainWindow::showHelp() {
  auto window = AboutWindow(this);
  window.exec();
}

void MainWindow::openTmpDir() {
  QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::tempPath()));
}
