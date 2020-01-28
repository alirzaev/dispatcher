#pragma once

#include <QWidget>

class AbstractTaskBuilder : public QWidget {
  Q_OBJECT

public:
  explicit AbstractTaskBuilder(QWidget *parent = nullptr) : QWidget(parent) {}

  ~AbstractTaskBuilder() override {}
signals:
  void historyStateChanged();
};
