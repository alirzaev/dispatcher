#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <utils/tasks.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

  ~MainWindow();

  void openTasks();

  void saveTasks();

  void loadTasks(const std::vector<Utils::Task> &tasks);

  void createTasks();

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
