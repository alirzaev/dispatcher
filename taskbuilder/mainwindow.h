#pragma once

#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QPoint>
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

  void attachTask(AbstractTaskBuilder *taskWidget);

  void closeEvent(QCloseEvent *event) override;

  void undoAction();

  void redoAction();

  void updateMenuEditState();

  void updateTaskPreview(int index);

  void processContextMenuAction(const QString &action, int taskIndex);

  void provideContextMenu(const QPoint &pos);

private:
  Ui::MainWindow *ui;
};
