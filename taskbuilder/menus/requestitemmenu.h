#pragma once

#include <QMenu>
#include <QString>
#include <QWidget>

class RequestItemMenu : public QMenu {
public:
  static QString TO_TOP;

  static QString TO_BOTTOM;

  static QString MOVE_UP;

  static QString MOVE_DOWN;

  static QString DELETE;

  RequestItemMenu(bool itemPresented, QWidget *parent = nullptr);
};
