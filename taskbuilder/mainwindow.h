#pragma once

#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QString>

#include <utils/tasks.h>

#include "abstracttaskbuilder.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

  ~MainWindow() override;

  void openTasks();

  void saveTasks();

  void loadTasks(const std::vector<Utils::Task> &tasks);

  void createTask(QAction *action);

  void removeTasks();

  void attachTask(AbstractTaskBuilder *taskWidget, const QString label);

  void closeEvent(QCloseEvent *event) override;

  void undoAction();

  void redoAction();

  void updateMenuEditState();

  void updateTaskPreview();

private:
  Ui::MainWindow *ui;
};
