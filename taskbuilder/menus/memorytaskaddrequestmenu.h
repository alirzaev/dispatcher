#pragma once

#include <QString>
#include <QWidget>

#include "requestitemmenu.h"

class MemoryTaskAddRequestMenu : public RequestItemMenu {
public:
  static QString CREATE_PROCESS;

  static QString TERMINATE_PROCESS;

  static QString ALLOCATE_MEMORY;

  static QString FREE_MEMORY;

  MemoryTaskAddRequestMenu(bool itemPresented, QWidget *parent = nullptr);
};
