#include <QIcon>

#include <qtutils/literals.h>

#include "memoryblockitem.h"

using namespace QtUtils::Literals;

MemoryBlockItem::MemoryBlockItem(const MemoryManagement::MemoryBlock &block,
                                 bool highlightFree,
                                 QListWidget *parent)
    : QListWidgetItem(parent), _block(block) {
  setText("address: %1; size: %2; pid: %3"_qs.arg(block.address(), 3)
              .arg(block.size(), 3)
              .arg(block.pid(), 3));
  if (highlightFree) {
    if (block.pid() == -1) {
      setIcon(QIcon(":/g/images/green_circle.png"));
    } else {
      setIcon(QIcon(":/g/images/red_circle.png"));
    }
  }
}

const MemoryManagement::MemoryBlock &MemoryBlockItem::block() const {
  return _block;
}
