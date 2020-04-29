#pragma once

#include <QMenu>
#include <QString>
#include <QWidget>

#include <tl/optional.hpp>

#include <algo/processes/types.h>

class ProcessMenu : public QMenu {
public:
  static QString CREATE;

  static QString TERMINATE;

  static QString TO_EXECUTING;

  static QString TO_WAITING;

  static QString TO_ACTIVE;

  static QString DECREASE;

  ProcessMenu(tl::optional<ProcessesManagement::Process> process,
              bool showItemDecrease = false,
              QWidget *parent = nullptr);
};
