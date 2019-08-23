#pragma once

#include <optional>

#include <QMenu>
#include <QString>
#include <QWidget>

#include <algo/processes/types.h>

class ProcessMenu : public QMenu {
public:
  static QString CREATE;

  static QString TERMINATE;

  static QString TO_EXECUTING;

  static QString TO_WAITING;

  static QString TO_ACTIVE;

  ProcessMenu(std::optional<ProcessesManagement::Process> process,
              QWidget *parent = nullptr);
};
