#ifndef TASKTYPECHOOSER_H
#define TASKTYPECHOOSER_H

#include <utility>

#include <QDialog>
#include <QString>

namespace Ui {
class TaskTypeChooser;
}

class TaskTypeChooser : public QDialog {
  Q_OBJECT

public:
  explicit TaskTypeChooser(QWidget *parent = nullptr);

  ~TaskTypeChooser();

  std::pair<int, QString> data;

private:
  Ui::TaskTypeChooser *ui;

  int taskType;

  QString strategy;

  void loadStrategies(int taskType);

  void tryAccept();
};

#endif // TASKTYPECHOOSER_H
