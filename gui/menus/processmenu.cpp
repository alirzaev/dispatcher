#include <QString>

#include "../literals.h"

#include "processmenu.h"

using namespace Utils::Literals;

ProcessMenu::ProcessMenu(QWidget *parent) : QMenu(parent) {
  addAction(CREATE);
  addAction(TERMINATE);
  addAction(TO_EXECUTING);
  addAction(TO_WAITING);
  addAction(TO_ACTIVE);
}

QString ProcessMenu::CREATE = "Добавить"_qs;

QString ProcessMenu::TERMINATE = "Удалить"_qs;

QString ProcessMenu::TO_EXECUTING = "Переключиться"_qs;

QString ProcessMenu::TO_WAITING = "Переключить в состояние ожидания"_qs;

QString ProcessMenu::TO_ACTIVE = "Переключить в состояние готовности"_qs;
