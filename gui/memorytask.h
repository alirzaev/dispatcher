#ifndef MEMORYTASK_H
#define MEMORYTASK_H

#include <QWidget>

namespace Ui {
class MemoryTask;
}

class MemoryTask : public QWidget
{
    Q_OBJECT

public:
    explicit MemoryTask(QWidget *parent = nullptr);
    ~MemoryTask();

private:
    Ui::MemoryTask *ui;
};

#endif // MEMORYTASK_H
