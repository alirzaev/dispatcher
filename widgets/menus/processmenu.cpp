#include <QIcon>
#include <QString>

#include <tl/optional.hpp>

#include <algo/processes/types.h>

#include <qtutils/literals.h>

#include "processmenu.h"

using namespace QtUtils::Literals;
using namespace ProcessesManagement;

extern void initResources();

ProcessMenu::ProcessMenu(tl::optional<Process> process, QWidget *parent)
    : QMenu(parent) {
  initResources();

  addAction(CREATE);
  addAction(TERMINATE)->setEnabled(process.has_value());
  addAction(QIcon(":/g/images/green_circle.png"), TO_EXECUTING)
      ->setEnabled(process.has_value() &&
                   process->state() == ProcState::ACTIVE);
  addAction(QIcon(":/g/images/red_circle.png"), TO_WAITING)
      ->setEnabled(process.has_value());
  addAction(QIcon(":/g/images/yellow_circle.png"), TO_ACTIVE)
      ->setEnabled(process.has_value());
}

QString ProcessMenu::CREATE = "Создать"_qs;

QString ProcessMenu::TERMINATE = "Завершить"_qs;

QString ProcessMenu::TO_EXECUTING = "Переключиться"_qs;

QString ProcessMenu::TO_WAITING = "Переключить в состояние ожидания"_qs;

QString ProcessMenu::TO_ACTIVE = "Переключить в состояние готовности"_qs;
