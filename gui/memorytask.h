#ifndef MEMORYTASK_H
#define MEMORYTASK_H

#include <QWidget>
#include <QPoint>

#include "views.h"
#include "../schedulers/algo/memory/types.h"

namespace Ui {
class MemoryTask;
}

class MemoryTask : public QWidget, public Views::MemoryTaskView
{
    Q_OBJECT

public:
    explicit MemoryTask(QWidget *parent = nullptr);

    void onAllocateAction(OnAllocateActionListener listener) override;

    void setMemoryBlocks(const std::vector<MemoryManagement::Types::MemoryBlock>& blocks) override;

    void setFreeMemoryBlocks(const std::vector<MemoryManagement::Types::MemoryBlock>& blocks) override;

    void setRequest(MemoryManagement::Requests::RequestPtr request) override;

    ~MemoryTask() override;

private:
    Ui::MemoryTask *ui;

    OnAllocateActionListener allocateActionListener;

    void provideContextMenu(const QPoint& pos);

    MemoryManagement::Types::MemoryState collectState();

    void processActionAllocate(const MemoryManagement::Types::MemoryBlock& block, int row);
};

#endif // MEMORYTASK_H
