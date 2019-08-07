#include <QDebug>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPoint>
#include <QPushButton>
#include <QRegExp>
#include <QString>

#include <cstdint>
#include <variant>
#include <vector>

#include <algo/memory/requests.h>
#include <algo/memory/strategies.h>
#include <algo/memory/types.h>
#include <utils/overload.h>

#include "literals.h"

#include "dialogs/allocatememorydialog.h"
#include "listitems/memoryblockitem.h"
#include "menus/memoryblockmenu.h"

#include "memorytask.h"
#include "ui_memorytask.h"

using namespace std::literals;
using namespace MemoryManagement;
using namespace Utils::Literals;

using std::vector;

MemoryTask::MemoryTask(Models::MemoryModel model, QWidget *parent)
    : QWidget(parent), ui(new Ui::MemoryTask), _model(model) {
  ui->setupUi(this);

  ui->listMemBlocks->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(ui->listMemBlocks,
          &QListWidget::customContextMenuRequested,
          this,
          &MemoryTask::provideContextMenu);
  connect(
      ui->acceptRequest, &QPushButton::clicked, this, &MemoryTask::nextRequest);
  connect(ui->resetState, &QPushButton::clicked, this, [=]() {
    _model.state = _model.task.state();
    refresh();
  });
  connect(ui->listFreeBlocks,
          &ReorderableListWidget::itemsOrderChanged,
          this,
          [=]() {
            _model.state = collectState();
            refresh();
          });

  refresh();
}

MemoryTask::~MemoryTask() { delete ui; }

void MemoryTask::setMemoryBlocks(const vector<MemoryBlock> &blocks) {
  auto *list = ui->listMemBlocks;
  list->clear();

  for (const auto &block : blocks) {
    list->addItem(new MemoryBlockItem(block, true));
  }
}

void MemoryTask::setFreeMemoryBlocks(const vector<MemoryBlock> &blocks) {
  auto *list = ui->listFreeBlocks;
  list->clear();

  for (const auto &block : blocks) {
    list->addItem(new MemoryBlockItem(block));
  }
}

void MemoryTask::setRequest(const Request &request) {
  auto *label = ui->labelRequestDescr;

  std::visit(
      overload{[label](const CreateProcessReq &req) {
                 label->setText("Создан новый процесс PID = %1. "
                                "Для размещения процесса в памяти (включая "
                                "служебную информацию) требуется выделить "
                                "%2 байт (%3 параграфов)"_qs.arg(req.pid())
                                    .arg(req.bytes())
                                    .arg(req.pages()));
               },
               [label](const TerminateProcessReq &req) {
                 label->setText("Процесс PID = %1 завершен"_qs.arg(req.pid()));
               },
               [label](const AllocateMemory &req) {
                 label->setText(
                     "Процесс PID = %1 выдал запрос на выделение ему "
                     "%2 байт (%3 параграфов) оперативной памяти"_qs
                         .arg(req.pid())
                         .arg(req.bytes())
                         .arg(req.pages()));
               },
               [label](const FreeMemory &req) {
                 label->setText("Процесс PID = %1 выдал запрос на освобождение "
                                "блока памяти с адресом %2"_qs.arg(req.pid())
                                    .arg(req.address()));
               }},
      request);
}

void MemoryTask::setStrategy(StrategyType type) {
  auto *label = ui->strategyLabel;

  if (type == StrategyType::FIRST_APPROPRIATE) {
    label->setText("Стратегия: первый подходящий");
  } else if (type == StrategyType::MOST_APPROPRIATE) {
    label->setText("Стратегия: наиболее подходящий");
  } else if (type == StrategyType::LEAST_APPROPRIATE) {
    label->setText("Стратегия: наименее подходящий");
  }
}

void MemoryTask::provideContextMenu(const QPoint &pos) {
  qDebug() << "ContextMenu";

  auto globalPos = ui->listMemBlocks->mapToGlobal(pos);
  auto listItem = ui->listMemBlocks->itemAt(pos);
  auto selectedBlock = dynamic_cast<MemoryBlockItem *>(listItem);
  auto row = ui->listMemBlocks->indexAt(pos).row();
  auto blockIndex = static_cast<uint32_t>(row);

  if (row == -1 || !selectedBlock) {
    return;
  }
  qDebug() << "ContextMenu:" << selectedBlock->text() << row;

  MemoryBlockMenu menu(selectedBlock->block());

  auto action = menu.exec(globalPos);
  if (!action) {
    return;
  }

  auto block = selectedBlock->block();

  if (action->text() == MemoryBlockMenu::ACTION_ALLOCATE) {
    qDebug() << "ContextMenu: allocate";
    processActionAllocate(block, blockIndex);
  } else if (action->text() == MemoryBlockMenu::ACTION_FREE) {
    qDebug() << "ContextMenu: free";
    processActionFree(block, blockIndex);
  } else if (action->text() == MemoryBlockMenu::ACTION_COMPRESS) {
    qDebug() << "ContextMenu: compress";
    processActionCompress(blockIndex);
  } else if (action->text() == MemoryBlockMenu::ACTION_DEFRAGMENT) {
    qDebug() << "ContextMenu: defragment";
    processActionDefragment();
  }
}

MemoryState MemoryTask::collectState() {
  vector<MemoryBlock> blocks, freeBlocks;

  for (int i = 0; i < ui->listMemBlocks->count(); ++i) {
    auto *item = dynamic_cast<MemoryBlockItem *>(ui->listMemBlocks->item(i));
    blocks.push_back(item->block());
  }

  for (int i = 0; i < ui->listFreeBlocks->count(); ++i) {
    auto *item = dynamic_cast<MemoryBlockItem *>(ui->listFreeBlocks->item(i));
    freeBlocks.push_back(item->block());
  }

  return {blocks, freeBlocks};
}

void MemoryTask::processActionAllocate(const MemoryBlock &block,
                                       uint32_t blockIndex) {
  auto dialog = AllocateMemoryDialog(this, block.size());
  auto res = dialog.exec();

  if (res == QDialog::Accepted) {
    auto [pid, size] = dialog.data;

    try {
      _model.state = collectState();
      _model.state = allocateMemory(_model.state, blockIndex, pid, size);
      refresh();
    } catch (const OperationException &ex) {
      if (ex.what() == "BLOCK_IS_USED"s) {
        showErrorMessage("Данный блок памяти уже выделен другому процессу");
      } else if (ex.what() == "TOO_SMALL"s) {
        showErrorMessage(
            "Процесс нельзя поместить в свободный блок, содержащий столько же "s +
            "парагрфов, сколько требуется данному процессу"s);
      } else {
        throw;
      }
    } catch (const std::exception &ex) {
      showErrorMessage("Неизвестная ошибка: "s + ex.what());
    }
  }
}

void MemoryTask::processActionFree(const MemoryBlock &block,
                                   uint32_t blockIndex) {
  auto pid = block.pid();

  try {
    _model.state = collectState();
    _model.state = freeMemory(_model.state, pid, blockIndex);
    refresh();
  } catch (const std::exception &ex) {
    showErrorMessage("Неизвестная ошибка: "s + ex.what());
  }
}

void MemoryTask::processActionDefragment() {
  try {
    _model.state = collectState();
    _model.state = defragmentMemory(_model.state);
    refresh();
  } catch (const std::exception &ex) {
    showErrorMessage("Неизвестная ошибка: "s + ex.what());
  }
}

void MemoryTask::processActionCompress(uint32_t blockIndex) {
  try {
    _model.state = collectState();
    _model.state = compressMemory(_model.state, blockIndex);
    refresh();
  } catch (const OperationException &ex) {
    if (ex.what() == "SINGLE_BLOCK"s) {
      showErrorMessage("Следующий блок свободен или отсутствует");
    } else {
      throw;
    }
  } catch (const std::exception &ex) {
    showErrorMessage("Неизвестная ошибка: "s + ex.what());
  }
}

void MemoryTask::refresh() {
  auto [blocks, freeBlocks] = _model.state;
  setMemoryBlocks(blocks);
  setFreeMemoryBlocks(freeBlocks);
  setStrategy(_model.task.strategy()->type);
  if (_model.task.done()) {
    setRequest(_model.task.requests().back());
  } else {
    auto index = _model.task.completed();
    setRequest(_model.task.requests()[index]);
  }
}

void MemoryTask::nextRequest() {
  _model.state = collectState();

  if (auto task = _model.task.next(_model.state); task.has_value()) {
    _model.task = task.value();
    _model.state = _model.task.state();
    refresh();
    if (_model.task.done()) {
      showInfoMessage("Вы успешно выполнили данное задание");
    }
  } else {
    showErrorMessage("Заявка обработана неверно");
  }
}

void MemoryTask::showErrorMessage(const std::string &message) {
  QMessageBox::critical(this, "Ошибка", QString::fromStdString(message));
}

void MemoryTask::showInfoMessage(const std::string &message) {
  QMessageBox::information(this, "Внимание", QString::fromStdString(message));
}

Models::MemoryModel MemoryTask::model() const { return _model; }
