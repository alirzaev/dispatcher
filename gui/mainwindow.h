#pragma once

#include <QMainWindow>

#include "views.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public Views::MainWindowView {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

  void onOpenTaskListener(OnOpenListener listener) override;

  void onSaveTaskListener(OnSaveListener listener) override;

  void onGenerateTaskListener(OnGenerateListener listener);

  std::vector<Views::TaskView>
  createTaskViews(const std::vector<Utils::Tasks::Task> &tasks) override;

  ~MainWindow() override;

private:
  Ui::MainWindow *ui;

  OnOpenListener openTaskListener;

  OnSaveListener saveTaskListener;

  OnGenerateListener generateTaskListener;

  void openTaskDialog();

  void saveTaskDialog();

  void generateTaskDialog();
};
