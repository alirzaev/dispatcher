#include "memoryblockmenu.h"

MemoryBlockMenu::MemoryBlockMenu(const MemoryManagement::Types::MemoryBlock &block, QWidget *parent) :
    QMenu (parent)
{
    addAction(ACTION_ALLOCATE)->setEnabled(block.pid() == -1);
    addAction(ACTION_FREE)->setEnabled(block.pid() != -1);
    addAction(ACTION_COMPRESS);
    addAction(ACTION_DEFRAGMENT);
}

QString MemoryBlockMenu::ACTION_ALLOCATE = "Выделить приложению";

QString MemoryBlockMenu::ACTION_FREE = "Освободить";

QString MemoryBlockMenu::ACTION_COMPRESS = "Объединить со следующим";

QString MemoryBlockMenu::ACTION_DEFRAGMENT = "Уплотнение памяти";
