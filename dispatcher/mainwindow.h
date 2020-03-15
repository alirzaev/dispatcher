#pragma once

#include <vector>

#include <QCloseEvent>
#include <QMainWindow>
#include <QString>
#include <QWidget>

#include <utils/tasks.h>

#include "models.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(const QString &student, QWidget *parent = nullptr);

  ~MainWindow() override;

private:
  Ui::MainWindow *ui;

  QString student;

  void openTasks();

  void saveTasks();

  void loadTasks(const std::vector<Utils::Task> &tasks);

  void createTasks();

  void dumpTasks(const std::vector<Utils::Task> &tasks);

  void showHelp();

  void openTmpDir();

  void closeEvent(QCloseEvent *event) override;
};
