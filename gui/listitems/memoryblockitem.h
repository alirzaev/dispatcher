#pragma once

#include <QListWidget>
#include <QWidget>

#include <algo/memory/types.h>

class MemoryBlockItem : public QListWidgetItem {
private:
  MemoryManagement::Types::MemoryBlock _block;

public:
  MemoryBlockItem(const MemoryManagement::Types::MemoryBlock &block,
                  QListWidget *parent = nullptr);

  const MemoryManagement::Types::MemoryBlock &block() const;
};
