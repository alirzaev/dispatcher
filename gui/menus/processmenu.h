#pragma once

#include <QMenu>
#include <QString>
#include <QWidget>

class ProcessMenu : public QMenu {
public:
  static QString CREATE;

  static QString TERMINATE;

  static QString TO_EXECUTING;

  static QString TO_WAITING;

  static QString TO_ACTIVE;

  ProcessMenu(int row, QWidget *parent = nullptr);
};
