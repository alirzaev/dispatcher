#include <QString>

#include <qtutils/literals.h>

#include "processtaskaddrequestmenu.h"

using namespace QtUtils::Literals;

ProcessTaskAddRequestMenu::ProcessTaskAddRequestMenu(bool itemPresented,
                                                     QWidget *parent)
    : RequestItemMenu(itemPresented, parent) {
  auto *menu = new QMenu("Добавить", this);
  menu->addAction(CREATE_PROCESS);
  menu->addAction(TERMINATE_PROCESS);
  menu->addAction(INIT_IO);
  menu->addAction(TERMINATE_IO);
  menu->addAction(TRANSFER_CONTROL);
  menu->addAction(TIME_QUANTUM_EXPIRED);
  this->addMenu(menu);
}

QString ProcessTaskAddRequestMenu::CREATE_PROCESS = "Создать процесс"_qs;

QString ProcessTaskAddRequestMenu::TERMINATE_PROCESS = "Завершить процесс"_qs;

QString ProcessTaskAddRequestMenu::INIT_IO = "Запрос на I/O"_qs;

QString ProcessTaskAddRequestMenu::TERMINATE_IO = "Завершить I/O"_qs;

QString ProcessTaskAddRequestMenu::TRANSFER_CONTROL = "Передать управление"_qs;

QString ProcessTaskAddRequestMenu::TIME_QUANTUM_EXPIRED =
    "Квант времени истек"_qs;
