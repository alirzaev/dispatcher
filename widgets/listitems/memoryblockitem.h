#pragma once

#include <QListWidget>
#include <QWidget>

#include <algo/memory/types.h>

class MemoryBlockItem : public QListWidgetItem {
private:
  MemoryManagement::MemoryBlock _block;

public:
  explicit MemoryBlockItem(const MemoryManagement::MemoryBlock &block,
                  bool highlightFree = false,
                  QListWidget *parent = nullptr);

  const MemoryManagement::MemoryBlock &block() const;
};
