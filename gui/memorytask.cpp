#include <QString>
#include <QMenu>
#include <QDebug>
#include <QRegExp>
#include <QMessageBox>
#include <QPushButton>

#include <vector>

#include "memorytask.h"
#include "ui_memorytask.h"
#include "dialogs/allocatememorydialog.h"
#include "listitems/memoryblockitem.h"
#include "menus/memoryblockmenu.h"

using namespace MemoryManagement;

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
    connect(ui->acceptRequest, &QPushButton::clicked, this, [=]() {
        if (nextRequestListener) nextRequestListener(collectState());
    });
    connect(ui->resetState, &QPushButton::clicked, this, [=]() {
        if (resetStateListener) resetStateListener();
    });
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
        list->addItem(new MemoryBlockItem(block));
    }
}

void MemoryTask::setFreeMemoryBlocks(const std::vector<MemoryManagement::Types::MemoryBlock>& blocks)
{
    auto* list = ui->listFreeBlocks;
    list->clear();
    for (const auto& block : blocks)
    {
        list->addItem(new MemoryBlockItem(block));
    }
}

void MemoryTask::setRequest(MemoryManagement::Requests::RequestPtr request)
{
    auto* label = ui->labelRequestDescr;
    if (request->type == Requests::RequestType::CREATE_PROCESS)
    {
        auto obj = *dynamic_cast<Requests::CreateProcess*>(request.get());
        label->setText(createProcessDescr.arg(obj.pid).arg(obj.bytes).arg(obj.pages));
    }
    else if (request->type == Requests::RequestType::TERMINATE_PROCESS)
    {
        auto obj = *dynamic_cast<Requests::TerminateProcess*>(request.get());
        label->setText(terminateProcessDescr.arg(obj.pid));
    }
    else if (request->type == Requests::RequestType::ALLOCATE_MEMORY)
    {
        auto obj = *dynamic_cast<Requests::AllocateMemory*>(request.get());
        label->setText(allocateMemoryDescr.arg(obj.pid).arg(obj.bytes).arg(obj.pages));
    }
    else if (request->type == Requests::RequestType::FREE_MEMORY)
    {
        auto obj = *dynamic_cast<Requests::FreeMemory*>(request.get());
        label->setText(freeMemoryDescr.arg(obj.pid).arg(obj.address));
    }
}

void MemoryTask::provideContextMenu(const QPoint& pos)
{
    auto globalPos = ui->listMemBlocks->mapToGlobal(pos);
    auto selectedBlock = dynamic_cast<MemoryBlockItem*>(ui->listMemBlocks->itemAt(pos));
    auto row = ui->listMemBlocks->indexAt(pos).row();

    if (row == -1 || !selectedBlock)
    {
        return;
    }
    qDebug() << selectedBlock->text() << row;

    MemoryBlockMenu menu(selectedBlock->block());

    auto action = menu.exec(globalPos);
    if (!action)
    {
        return;
    }

    auto block = selectedBlock->block();

    if (action->text() == MemoryBlockMenu::ACTION_ALLOCATE)
    {
        qDebug() << "allocate";
        processActionAllocate(block, row);
    }
    else if (action->text() == MemoryBlockMenu::ACTION_FREE)
    {
        qDebug() << "free";
        processActionFree(block, row);
    }
    else if (action->text() == MemoryBlockMenu::ACTION_COMPRESS)
    {
        qDebug() << "compress";
        processActionCompress(block, row);
    }
    else if (action->text() == MemoryBlockMenu::ACTION_DEFRAGMENT)
    {
        qDebug() << "defragment";
        processActionDefragment(block, row);
    }
}

MemoryState MemoryTask::collectState()
{
    using MemoryManagement::Types::MemoryBlock;
    using std::vector;

    vector<MemoryBlock> blocks, freeBlocks;

    for (int i = 0; i < ui->listMemBlocks->count(); ++i)
    {
        auto* item = dynamic_cast<MemoryBlockItem*>(ui->listMemBlocks->item(i));
        blocks.push_back(item->block());
    }

    for (int i = 0; i < ui->listFreeBlocks->count(); ++i)
    {
        auto* item = dynamic_cast<MemoryBlockItem*>(ui->listFreeBlocks->item(i));
        freeBlocks.push_back(item->block());
    }

    return {blocks, freeBlocks};
}

void MemoryTask::processActionAllocate(const MemoryBlock &block, int row)
{
    auto dialog = AllocateMemoryDialog(this, block.size());
    auto res = dialog.exec();
    if (res == QDialog::Accepted && allocateActionListener)
    {
        auto [pid, size] = dialog.data;
        auto state = collectState();

        allocateActionListener({pid, size, row}, state);
    }
}

void MemoryTask::processActionFree(const MemoryBlock &block, int row)
{
    if (freeActionListener)
    {
        auto state = collectState();
        freeActionListener({block.pid(), row}, state);
    }
}

void MemoryTask::processActionDefragment(const MemoryBlock &block, int row)
{
    if (defragmentActionListener)
    {
        auto state = collectState();
        defragmentActionListener(state);
    }
}

void MemoryTask::processActionCompress(const MemoryBlock &block, int row)
{
    if (compressActionListener)
    {
        auto state = collectState();
        compressActionListener(row, state);
    }
}


void MemoryTask::onAllocateAction(OnAllocateActionListener listener)
{
    allocateActionListener = listener;
}


void MemoryTask::onFreeAction(OnFreeActionListener listener)
{
    freeActionListener = listener;
}


void MemoryTask::onDefragmentAction(OnDefragmentActionListener listener)
{
    defragmentActionListener = listener;
}


void MemoryTask::onCompressAction(OnCompressActionListener listener)
{
    compressActionListener = listener;
}

void MemoryTask::onNextRequestListener(OnNextRequestListener listener)
{
    nextRequestListener = listener;
}

void MemoryTask::onResetStateListener(OnResetStateListener listener)
{
    resetStateListener = listener;
}

void MemoryTask::showErrorMessage(const std::string &message)
{
    QMessageBox::critical(this, "Ошибка", QString::fromStdString(message));
}

void MemoryTask::showInfoMessage(const std::string &message)
{
    QMessageBox::information(this, "Внимание", QString::fromStdString(message));
}
