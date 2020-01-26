#include <QString>

#include <qtutils/literals.h>

#include "memorytaskaddrequestmenu.h"

using namespace QtUtils::Literals;

MemoryTaskAddRequestMenu::MemoryTaskAddRequestMenu(bool itemPresented,
                                                   QWidget *parent)
    : RequestItemMenu(itemPresented, parent) {
  auto *menu = new QMenu("Добавить", this);
  menu->addAction(CREATE_PROCESS);
  menu->addAction(TERMINATE_PROCESS);
  menu->addAction(ALLOCATE_MEMORY);
  menu->addAction(FREE_MEMORY);
  this->addMenu(menu);
}

QString MemoryTaskAddRequestMenu::CREATE_PROCESS = "Создать процесс"_qs;

QString MemoryTaskAddRequestMenu::TERMINATE_PROCESS = "Завершить процесс"_qs;

QString MemoryTaskAddRequestMenu::ALLOCATE_MEMORY = "Выделить память"_qs;

QString MemoryTaskAddRequestMenu::FREE_MEMORY = "Освободить память"_qs;
