#include "memorytask.h"
#include "ui_memorytask.h"

MemoryTask::MemoryTask(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MemoryTask)
{
    ui->setupUi(this);
}

MemoryTask::~MemoryTask()
{
    delete ui;
}
