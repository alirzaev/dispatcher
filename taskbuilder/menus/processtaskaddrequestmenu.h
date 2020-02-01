#pragma once

#include <QString>
#include <QWidget>

#include "requestitemmenu.h"

class ProcessTaskAddRequestMenu : public RequestItemMenu {
public:
  static QString CREATE_PROCESS;

  static QString TERMINATE_PROCESS;

  static QString INIT_IO;

  static QString TERMINATE_IO;

  static QString TRANSFER_CONTROL;

  static QString TIME_QUANTUM_EXPIRED;

  ProcessTaskAddRequestMenu(bool itemPresented, QWidget *parent = nullptr);
};
