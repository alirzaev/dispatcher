#include <QIcon>
#include <QString>

#include <tl/optional.hpp>

#include <algo/processes/types.h>

#include "../literals.h"

#include "processmenu.h"

using namespace Utils::Literals;
using namespace ProcessesManagement;

ProcessMenu::ProcessMenu(tl::optional<Process> process, QWidget *parent)
    : QMenu(parent) {
  addAction(CREATE);
  addAction(TERMINATE)->setEnabled(process.has_value());
  addAction(QIcon(":/g/images/green_circle.svg"), TO_EXECUTING)
      ->setEnabled(process.has_value() &&
                   process->state() == ProcState::ACTIVE);
  addAction(QIcon(":/g/images/red_circle.svg"), TO_WAITING)
      ->setEnabled(process.has_value());
  addAction(QIcon(":/g/images/yellow_circle.svg"), TO_ACTIVE)
      ->setEnabled(process.has_value());
}

QString ProcessMenu::CREATE = "Создать"_qs;

QString ProcessMenu::TERMINATE = "Завершить"_qs;

QString ProcessMenu::TO_EXECUTING = "Переключиться"_qs;

QString ProcessMenu::TO_WAITING = "Переключить в состояние ожидания"_qs;

QString ProcessMenu::TO_ACTIVE = "Переключить в состояние готовности"_qs;
