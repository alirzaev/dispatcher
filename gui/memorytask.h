#ifndef MEMORYTASK_H
#define MEMORYTASK_H

#include <QWidget>

#include "views.h"

namespace Ui {
class MemoryTask;
}

class MemoryTask : public QWidget, public Views::MemoryTaskView
{
    Q_OBJECT

public:
    explicit MemoryTask(QWidget *parent = nullptr);

    void setMemoryBlocks(const std::vector<MemoryManagement::Types::MemoryBlock>& blocks) override;

    void setFreeMemoryBlocks(const std::vector<MemoryManagement::Types::MemoryBlock>& blocks) override;

    void setRequest(MemoryManagement::Requests::RequestPtr request) override;

    ~MemoryTask();

private:
    Ui::MemoryTask *ui;
};

#endif // MEMORYTASK_H
