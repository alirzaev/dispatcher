#pragma once

#include <QMenu>
#include <QWidget>

#include <algo/memory/types.h>

class MemoryBlockMenu : public QMenu {
public:
  static QString ACTION_ALLOCATE;

  static QString ACTION_FREE;

  static QString ACTION_COMPRESS;

  static QString ACTION_DEFRAGMENT;

  MemoryBlockMenu(const MemoryManagement::Types::MemoryBlock &block,
                  QWidget *parent = nullptr);
};
