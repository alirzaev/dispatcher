#include <QString>

#include "memoryblockitem.h"

MemoryBlockItem::MemoryBlockItem(const MemoryManagement::MemoryBlock &block,
                                 bool highlightFree, QListWidget *parent)
    : QListWidgetItem(parent), _block(block) {
  auto blockRepr = QString("address: %1; size: %2; pid: %3")
                       .arg(block.address(), 3)
                       .arg(block.size(), 3)
                       .arg(block.pid(), 3);
  if (highlightFree) {
    if (block.pid() == -1) {
      blockRepr.prepend("* ");
    } else {
      blockRepr.prepend("  ");
    }
  }
  setText(blockRepr);
}

const MemoryManagement::MemoryBlock &MemoryBlockItem::block() const {
  return _block;
}
