#pragma once

#include <QString>
#include <QWidget>

#include <utils/tasks.h>

class AbstractTaskBuilder : public QWidget {
  Q_OBJECT

public:
  explicit AbstractTaskBuilder(QWidget *parent = nullptr) : QWidget(parent) {}

  ~AbstractTaskBuilder() override {}

  virtual QString strategy() = 0;

  virtual Utils::Task task() = 0;

signals:
  void historyStateChanged();
};
