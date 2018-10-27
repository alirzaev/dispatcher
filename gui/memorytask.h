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

    void onFreeAction(OnFreeActionListener listener) override;

    void onDefragmentAction(OnDefragmentActionListener listener) override;

    void onCompressAction(OnCompressActionListener listener) override;

    void onNextRequestListener(OnNextRequestListener listener) override;

    void onResetStateListener(OnResetStateListener listener) override;

    void setMemoryBlocks(const std::vector<MemoryManagement::Types::MemoryBlock>& blocks) override;

    void setFreeMemoryBlocks(const std::vector<MemoryManagement::Types::MemoryBlock>& blocks) override;

    void setRequest(MemoryManagement::Requests::RequestPtr request) override;

    void showErrorMessage(const std::string &message) override;

    ~MemoryTask() override;

private:
    Ui::MemoryTask *ui;

    OnAllocateActionListener allocateActionListener;

    OnFreeActionListener freeActionListener;

    OnDefragmentActionListener defragmentActionListener;

    OnCompressActionListener compressActionListener;

    OnNextRequestListener nextRequestListener;

    OnResetStateListener resetStateListener;

    void provideContextMenu(const QPoint& pos);

    MemoryManagement::Types::MemoryState collectState();

    void processActionAllocate(const MemoryManagement::Types::MemoryBlock& block, int row);

    void processActionFree(const MemoryManagement::Types::MemoryBlock& block, int row);

    void processActionDefragment(const MemoryManagement::Types::MemoryBlock& block, int row);

    void processActionCompress(const MemoryManagement::Types::MemoryBlock& block, int row);
};

#endif // MEMORYTASK_H
