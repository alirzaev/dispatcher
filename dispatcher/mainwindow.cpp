#ifdef _WIN32
#include <filesystem>
#endif
#include <fstream>
#include <utility>
#include <vector>

#include <QAction>
#include <QCloseEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QUrl>

#include <mapbox/variant.hpp>

#include <botan/exceptn.h>

#include <generators/memory_task.h>
#include <generators/processes_task.h>
#include <utils/io.h>
#include <utils/tasks.h>

#include <qtutils/cryptography.h>
#include <qtutils/fileio.h>
#include <qtutils/literals.h>

#include "models.h"
#include "taskgetter.h"

#include "aboutwindow.h"
#include "memorytask.h"
#include "processestask.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace QtUtils::Literals;

MainWindow::MainWindow(const QString &student, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), student(student) {
  ui->setupUi(this);

  connect(
      ui->actionOpenTask, &QAction::triggered, this, &MainWindow::openTasks);
  connect(
      ui->actionSaveTask, &QAction::triggered, this, &MainWindow::saveTasks);

  connect(ui->actionGenerateTask,
          &QAction::triggered,
          this,
          &MainWindow::createTasks);
  connect(ui->actionQuit, &QAction::triggered, this, &QMainWindow::close);
  connect(
      ui->actionOpenTmpDir, &QAction::triggered, this, &MainWindow::openTmpDir);
  connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showHelp);

  setWindowTitle("%1 (%2)"_qs.arg(windowTitle()).arg(student));
  ui->actionSaveTask->setDisabled(true);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::openTasks() {
  auto fileName =
      QFileDialog::getOpenFileName(this,
                                   "Открыть файл задания",
                                   "",
                                   "Encrypted JSON (*.ejson);;JSON (*.json)");
  if (fileName.isEmpty()) {
    return;
  }

  bool decryptionRequired = false;
  if (fileName.endsWith(".ejson")) {
    decryptionRequired = true;
  }

  try {
    auto data = QtUtils::FileIO::readAll(fileName);
    if (data.empty()) {
      QMessageBox::warning(this, "Ошибка", "Невозможно открыть файл задания");
      return;
    }

    std::stringstream ss;
    if (decryptionRequired) {
      auto decrypted = QtUtils::Cryptography::decrypt(data, student);
      ss.str(decrypted);
    } else {
      ss.str(data);
    }

    std::vector<Utils::Task> tasks;
    unsigned int total_count = 0;
    for (auto task : Utils::loadTasks(ss)) {
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
    ui->actionSaveTask->setDisabled(false);
  } catch (const Botan::Exception &ex) {
    qCritical() << ex.what();
    QMessageBox::warning(
        this,
        "Ошибка",
        "Невозможно загрузить задания: файл принадлежит другому студенту");
  } catch (const std::exception &ex) {
    qCritical() << ex.what();
    QMessageBox::warning(
        this, "Ошибка", "Невозможно загрузить задания: файл поврежден");
  }
}

void MainWindow::saveTasks() {
  auto fileName = QFileDialog::getSaveFileName(
      this, "Сохранить задание в файл", "", "Encrypted JSON (*.ejson)");
  if (fileName.isEmpty()) {
    return;
  }

  if (!fileName.endsWith(".ejson")) {
    fileName.append(".ejson");
  }

  try {
    std::ofstream file = QtUtils::FileIO::openStdOfstream(fileName);

    std::vector<Utils::Task> tasks;

    for (int i = 0; i < ui->tabWidget->count(); ++i) {
      auto *widget = dynamic_cast<TaskGetter *>(ui->tabWidget->widget(i));
      auto task = widget->task();
      tasks.push_back(task);
    }

    std::stringstream ss;

    Utils::saveTasks(tasks, ss);

    auto encrypted = QtUtils::Cryptography::encrypt(ss.str(), student);
    file << encrypted;
    file.flush();
  } catch (const std::exception &ex) {
    qCritical() << ex.what();
    QMessageBox::warning(this, "Ошибка", "Невозможно сохранить задания");
  }
}

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
  ui->actionSaveTask->setDisabled(false);
}

void MainWindow::dumpTasks(const std::vector<Utils::Task> &tasks) {
  try {
    auto tempFilePath =
        QDir::toNativeSeparators(QDir::tempPath() + "/dispatcher.json");

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

void MainWindow::closeEvent(QCloseEvent *event) {
  auto answer = QMessageBox::question(
      this, "Завершение работы", "Вы действительно хотите выйти?");
  if (answer == QMessageBox::Yes) {
    event->accept();
  } else {
    event->ignore();
  }
}
