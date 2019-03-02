#pragma once

#include <vector>

#include <QMainWindow>
#include <QWidget>

#include <utils/tasks.h>

#include "models.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

  ~MainWindow() override;

private:
  Ui::MainWindow *ui;

  void openTasks();

  void loadTasks(const std::vector<Utils::Task> &tasks);

  void saveTasks();

  void createTasks();
};
