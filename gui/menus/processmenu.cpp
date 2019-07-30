#include <QString>

#include "../literals.h"

#include "processmenu.h"

using namespace Utils::Literals;

ProcessMenu::ProcessMenu(int row, QWidget *parent) : QMenu(parent) {
  addAction(CREATE);
  addAction(TERMINATE)->setEnabled(row != -1);
  addAction(TO_EXECUTING)->setEnabled(row != -1);
  addAction(TO_WAITING)->setEnabled(row != -1);
  addAction(TO_ACTIVE)->setEnabled(row != -1);
}

QString ProcessMenu::CREATE = "Добавить"_qs;

QString ProcessMenu::TERMINATE = "Удалить"_qs;

QString ProcessMenu::TO_EXECUTING = "Переключиться"_qs;

QString ProcessMenu::TO_WAITING = "Переключить в состояние ожидания"_qs;

QString ProcessMenu::TO_ACTIVE = "Переключить в состояние готовности"_qs;
