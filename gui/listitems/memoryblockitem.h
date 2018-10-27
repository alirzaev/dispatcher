#ifndef MEMORYBLOCKITEM_H
#define MEMORYBLOCKITEM_H

#include <QListWidget>
#include <QWidget>

#include "../schedulers/algo/memory/types.h"

class MemoryBlockItem : public QListWidgetItem
{
private:
    MemoryManagement::Types::MemoryBlock _block;

public:
    MemoryBlockItem(const MemoryManagement::Types::MemoryBlock& block, QListWidget* parent = nullptr);

    const MemoryManagement::Types::MemoryBlock& block() const;
};

#endif // MEMORYBLOCKITEM_H
