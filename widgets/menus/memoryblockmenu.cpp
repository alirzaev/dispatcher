#include <QString>

#include <qtutils/literals.h>

#include "memoryblockmenu.h"

using namespace QtUtils::Literals;

MemoryBlockMenu::MemoryBlockMenu(const MemoryManagement::MemoryBlock &block,
                                 QWidget *parent)
    : QMenu(parent) {
  addAction(ACTION_ALLOCATE)->setEnabled(block.pid() == -1);
  addAction(ACTION_FREE)->setEnabled(block.pid() != -1);
  addAction(ACTION_COMPRESS)->setEnabled(block.pid() == -1);
  addAction(ACTION_DEFRAGMENT);
}

QString MemoryBlockMenu::ACTION_ALLOCATE = "Выделить приложению"_qs;

QString MemoryBlockMenu::ACTION_FREE = "Освободить"_qs;

QString MemoryBlockMenu::ACTION_COMPRESS = "Объединить со следующим"_qs;

QString MemoryBlockMenu::ACTION_DEFRAGMENT = "Уплотнение памяти"_qs;
