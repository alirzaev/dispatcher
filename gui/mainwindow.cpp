#include <fstream>
#include <variant>
#include <vector>

#include <QAction>
#include <QFileDialog>

#include <generators/memory_task.h>
#include <generators/processes_task.h>
#include <utils/io.h>
#include <utils/overload.h>
#include <utils/tasks.h>

#include "models.h"

#include "memorytask.h"
#include "processestask.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->tabWidget->clear();

  connect(ui->actionOpenTask, &QAction::triggered, this,
          &MainWindow::openTasks);
  connect(ui->actionSaveTask, &QAction::triggered, this,
          &MainWindow::saveTasks);
  connect(ui->actionGenerateTask, &QAction::triggered, this,
          &MainWindow::createTasks);
  connect(ui->actionQuit, &QAction::triggered, this, &QMainWindow::close);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::openTasks() {
  auto fileName = QFileDialog::getOpenFileName(this, "Открыть файл задания", "",
                                               "JSON (*.json)");
  if (fileName.isEmpty()) {
    return;
  }

  std::ifstream file(fileName.toStdString());

  auto tasks = Utils::loadTasks(file);

  loadTasks(tasks);
}

void MainWindow::loadTasks(const std::vector<Utils::Task> &tasks) {
  auto *tabs = ui->tabWidget;
  tabs->clear();

  for (const auto &task : tasks) {
    std::visit(
        overload{[tabs, this](const Utils::MemoryTask &task) {
                   auto model = Models::MemoryModel{task.state(), task};

                   auto *taskWidget = new MemoryTask(model, this);
                   tabs->addTab(taskWidget, "Диспетчеризация памяти");
                 },
                 [tabs, this](const Utils::ProcessesTask &task) {
                   auto model = Models::ProcessesModel{task.state(), task};

                   auto *taskWidget = new ProcessesTask(model, this);
                   tabs->addTab(taskWidget, "Диспетчеризация процессов");
                 }},
        task);
  }
}

void MainWindow::saveTasks() {
  auto fileName = QFileDialog::getSaveFileName(this, "Сохранить задание в файл",
                                               "", "JSON (*.json)");
  if (fileName.isEmpty()) {
    return;
  }

  if (!fileName.endsWith(".json")) {
    fileName.append(".json");
  }

  std::ofstream file(fileName.toStdString());

  std::vector<Utils::Task> tasks;

  for (int i = 0; i < ui->tabWidget->count(); ++i) {
    auto *widget = ui->tabWidget->widget(i);
    if (auto *p = dynamic_cast<MemoryTask *>(widget); p != nullptr) {
      auto model = p->model();
      tasks.push_back(model.task);
    } else if (auto *p = dynamic_cast<ProcessesTask *>(widget); p != nullptr) {
      auto model = p->model();
      tasks.push_back(model.task);
    }
  }

  Utils::saveTasks(tasks, file);
}

void MainWindow::createTasks() {
  loadTasks({Generators::MemoryTask::generate(),
             Generators::ProcessesTask::generate()});
}
