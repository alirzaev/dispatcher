#include <QString>
#include <QMenu>
#include <QDebug>

#include "memorytask.h"
#include "ui_memorytask.h"

using namespace MemoryManagement;

class ContextMenu : public QMenu
{
public:
    static QString ACTION_ALLOCATE;

    static QString ACTION_FREE;

    static QString ACTION_COMPRESS;

    static QString ACTION_DEFRAGMENT;

    ContextMenu() :
        QMenu(nullptr)
    {
        addAction(ACTION_ALLOCATE);
        addAction(ACTION_FREE);
        addAction(ACTION_COMPRESS);
        addAction(ACTION_DEFRAGMENT);
    }
};

QString ContextMenu::ACTION_ALLOCATE = "Выделить приложению";

QString ContextMenu::ACTION_FREE = "Освободить";

QString ContextMenu::ACTION_COMPRESS = "Объединить со следующим";

QString ContextMenu::ACTION_DEFRAGMENT = "Уплотнение памяти";

QString createProcessDescr =
        "Создан новый процесс PID = %1. "
        "Для размещения процесса в памяти требуется выделить "
        "%2 байт (%3 параграфов)";

QString terminateProcessDescr = "Процесс PID = %1 завершен";

QString freeMemoryDescr =
        "Процесс PID = %1 выдал запрос на освобождение "
        "блока памяти с адресом %2";

QString allocateMemoryDescr =
        "Процесс PID = %1 выдал запрос на выделение ему "
        "%2 байт (%3 параграфов) оперативной памяти";

MemoryTask::MemoryTask(QWidget *parent) :
    QWidget(parent),
    Views::MemoryTaskView(),
    ui(new Ui::MemoryTask)
{
    ui->setupUi(this);

    ui->listMemBlocks->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listMemBlocks, &QListWidget::customContextMenuRequested,
            this, &MemoryTask::provideContextMenu);
}

MemoryTask::~MemoryTask()
{
    delete ui;
}

void MemoryTask::setMemoryBlocks(const std::vector<MemoryManagement::Types::MemoryBlock>& blocks)
{
    auto* list = ui->listMemBlocks;
    list->clear();
    for (const auto& block : blocks)
    {
        auto blockRepr = QString("address: %1; size: %2; pid: %3")
            .arg(block.address(), 3)
            .arg(block.size(), 3)
            .arg(block.pid(), 3);
        list->addItem(blockRepr);
    }
}

void MemoryTask::setFreeMemoryBlocks(const std::vector<MemoryManagement::Types::MemoryBlock>& blocks)
{
    auto* list = ui->listFreeBlocks;
    list->clear();
    for (const auto& block : blocks)
    {
        auto blockRepr = QString("address: %1; size: %2; pid: %3")
            .arg(block.address(), 3)
            .arg(block.size(), 3)
            .arg(block.pid(), 3);
        list->addItem(blockRepr);
    }
}

void MemoryTask::setRequest(MemoryManagement::Requests::RequestPtr request)
{
    auto* label = ui->labelRequestDescr;
    if (request->type == Requests::RequestType::CREATE_PROCESS) {
        auto obj = *dynamic_cast<Requests::CreateProcess*>(request.get());
        label->setText(createProcessDescr.arg(obj.pid).arg(obj.bytes).arg(obj.pages));
    } else if (request->type == Requests::RequestType::TERMINATE_PROCESS) {
        auto obj = *dynamic_cast<Requests::TerminateProcess*>(request.get());
        label->setText(terminateProcessDescr.arg(obj.pid));
    } else if (request->type == Requests::RequestType::ALLOCATE_MEMORY) {
        auto obj = *dynamic_cast<Requests::AllocateMemory*>(request.get());
        label->setText(allocateMemoryDescr.arg(obj.pid).arg(obj.bytes).arg(obj.pages));
    } else if (request->type == Requests::RequestType::FREE_MEMORY) {
        auto obj = *dynamic_cast<Requests::FreeMemory*>(request.get());
        label->setText(freeMemoryDescr.arg(obj.pid).arg(obj.address));
    }
}

void MemoryTask::provideContextMenu(const QPoint& pos)
{
    auto globalPos = ui->listMemBlocks->mapToGlobal(pos);
    auto selectedBlock = ui->listMemBlocks->itemAt(pos);

    qDebug() << selectedBlock->text();

    ContextMenu menu;

    auto action = menu.exec(globalPos);

    if (action->text() == ContextMenu::ACTION_ALLOCATE)
    {
        qDebug() << "allocate";
    }
    else if (action->text() == ContextMenu::ACTION_FREE)
    {
        qDebug() << "free";
    }
    else if (action->text() == ContextMenu::ACTION_COMPRESS)
    {
        qDebug() << "compress";
    }
    else if (action->text() == ContextMenu::ACTION_DEFRAGMENT)
    {
        qDebug() << "defragment";
    }
}
