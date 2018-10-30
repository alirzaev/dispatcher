#include "memoryblockitem.h"

#include <QString>

MemoryBlockItem::MemoryBlockItem(
    const MemoryManagement::Types::MemoryBlock &block, QListWidget *parent)
    : QListWidgetItem(parent), _block(block) {
  auto blockRepr = QString("address: %1; size: %2; pid: %3")
                       .arg(block.address(), 3)
                       .arg(block.size(), 3)
                       .arg(block.pid(), 3);
  setText(blockRepr);
}

const MemoryManagement::Types::MemoryBlock &MemoryBlockItem::block() const {
  return _block;
}
