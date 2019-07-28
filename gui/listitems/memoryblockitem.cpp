#include <QString>

#include "../literals.h"

#include "memoryblockitem.h"

using namespace Utils::Literals;

MemoryBlockItem::MemoryBlockItem(const MemoryManagement::MemoryBlock &block,
                                 bool highlightFree,
                                 QListWidget *parent)
    : QListWidgetItem(parent), _block(block) {
  auto blockRepr = "address: %1; size: %2; pid: %3"_qs.arg(block.address(), 3)
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
