#include <optional>

#include <QString>

#include <algo/processes/types.h>

#include "../literals.h"

#include "processmenu.h"

using namespace Utils::Literals;
using namespace ProcessesManagement;

ProcessMenu::ProcessMenu(std::optional<Process> process, QWidget *parent)
    : QMenu(parent) {
  addAction(CREATE);
  addAction(TERMINATE)->setEnabled(process.has_value());
  addAction(TO_EXECUTING)
      ->setEnabled(process.has_value() &&
                   process->state() == ProcState::ACTIVE);
  addAction(TO_WAITING)->setEnabled(process.has_value());
  addAction(TO_ACTIVE)->setEnabled(process.has_value());
}

QString ProcessMenu::CREATE = "Добавить"_qs;

QString ProcessMenu::TERMINATE = "Удалить"_qs;

QString ProcessMenu::TO_EXECUTING = "Переключиться"_qs;

QString ProcessMenu::TO_WAITING = "Переключить в состояние ожидания"_qs;

QString ProcessMenu::TO_ACTIVE = "Переключить в состояние готовности"_qs;
