#include <QString>

#include <qtutils/literals.h>

#include "requestitemmenu.h"

using namespace QtUtils::Literals;

RequestItemMenu::RequestItemMenu(bool itemPresented, QWidget *parent)
    : QMenu(parent) {
  addAction(TO_TOP)->setEnabled(itemPresented);
  addAction(MOVE_UP)->setEnabled(itemPresented);
  addAction(MOVE_DOWN)->setEnabled(itemPresented);
  addAction(TO_BOTTOM)->setEnabled(itemPresented);
  addAction(DELETE)->setEnabled(itemPresented);
}

QString RequestItemMenu::TO_TOP = "Переместить в начало"_qs;

QString RequestItemMenu::TO_BOTTOM = "Переместить в конец"_qs;

QString RequestItemMenu::MOVE_UP = "Переместить вверх"_qs;

QString RequestItemMenu::MOVE_DOWN = "Переместить вниз"_qs;

QString RequestItemMenu::DELETE = "Удалить"_qs;
