#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QCloseEvent>
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

  void createTask(QAction *action);

  void closeEvent(QCloseEvent *event) override;

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
