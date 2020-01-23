#include <QApplication>
#include <QDebug>
#include <QFont>
#include <QFontDatabase>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMessageBox>
#include <QPoint>
#include <QPushButton>
#include <QRegExp>
#include <QString>

#include <cstdint>
#include <vector>

#include <mapbox/variant.hpp>

#include <algo/memory/requests.h>
#include <algo/memory/strategies.h>
#include <algo/memory/types.h>

#include <qtutils/literals.h>

#include <dialogs/allocatememorydialog.h>
#include <listitems/memoryblockitem.h>
#include <menus/memoryblockmenu.h>
#include <reorderablelistwidget.h>

#include "memorytask.h"
#include "ui_memorytask.h"

#include "fontscale.h"

using namespace std::literals;
using namespace MemoryManagement;
using namespace QtUtils::Literals;

using std::vector;

static const QString ACTIONS_TEMPL("Действия пользователя:%1");

static const QString ACTIONS_UNAVAILABLE("\n<нет информации>");

static const QString NO_ACTIONS("\n<нет>");

MemoryTask::MemoryTask(Models::MemoryModel model, QWidget *parent)
    : QWidget(parent), _model(model), currentRequest(model.task.completed()),
      currentActions(""), actions(model.task.completed(), ""),
      states(model.task.requests().size()), ui(new Ui::MemoryTask) {
  ui->setupUi(this);

  auto font = QApplication::font();
  font.setPointSizeF(font.pointSizeF() * FONT_SCALE_FACTOR);
  ui->labelRequestDescr->setFont(font);

  auto fixedFont = QFont("Cascadia Mono");
  fixedFont.setPointSize(10);
  ui->listMemBlocks->setFont(fixedFont);
  ui->listFreeBlocks->setFont(fixedFont);

  ui->listMemBlocks->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(ui->listMemBlocks,
          &QListWidget::customContextMenuRequested,
          this,
          &MemoryTask::provideContextMenu);
  connect(
      ui->acceptRequest, &QPushButton::clicked, this, &MemoryTask::nextRequest);
  connect(ui->resetState, &QPushButton::clicked, this, [=]() {
    currentActions.clear();
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
  connect(ui->historyList,
          &QListWidget::currentRowChanged,
          this,
          &MemoryTask::updateCurrentRequest);

  auto state = MemoryState::initial();
  auto strategy = _model.task.strategy();
  for (size_t i = 0; i < _model.task.completed(); ++i) {
    auto request = _model.task.requests().at(i);
    state = strategy->processRequest(request, state);
    states[i] = state;
  }

  for (size_t i = 0; i < _model.task.completed(); ++i) {
    QString action;
    if (i < _model.task.actions().size()) {
      actions[i] = QString::fromStdString(_model.task.actions()[i]);
    } else {
      actions[i] = ACTIONS_UNAVAILABLE;
    }
  }

  updateHistoryView(_model.task, actions);
  refresh();
}

Utils::Task MemoryTask::task() const {
  std::vector<std::string> stdStrActions(actions.size());
  for (size_t i = 0; i < actions.size(); ++i) {
    stdStrActions[i] = actions[i].toStdString();
  }

  const auto &task = _model.task;
  return Utils::MemoryTask::create(task.strategy(),
                                   task.completed(),
                                   task.fails(),
                                   task.state(),
                                   task.requests(),
                                   stdStrActions);
}

MemoryTask::~MemoryTask() { delete ui; }

void MemoryTask::processActionAllocate(const MemoryBlock &block,
                                       uint32_t blockIndex) {
  auto info = AllocateMemoryDialog::getMemoryBlockInfo(this, block.size());

  if (!info) {
    return;
  }

  auto [pid, size] = *info;

  try {
    _model.state = collectState();
    _model.state = allocateMemory(_model.state, blockIndex, pid, size);
    currentActions += "\nВыделен блок памяти (address=%1, pages=%2, pid=%3)"_qs
                          .arg(block.address())
                          .arg(size)
                          .arg(pid);
    refresh();
  } catch (const std::exception &ex) {
    warning("Неизвестная ошибка: "s + ex.what());
  }
}

void MemoryTask::processActionFree(const MemoryBlock &block,
                                   uint32_t blockIndex) {
  auto pid = block.pid();

  try {
    _model.state = collectState();
    _model.state = freeMemory(_model.state, pid, blockIndex);
    currentActions +=
        "\nОсвобожден блок памяти (address=%1)"_qs.arg(block.address());
    refresh();
  } catch (const std::exception &ex) {
    warning("Неизвестная ошибка: "s + ex.what());
  }
}

void MemoryTask::processActionCompress(uint32_t blockIndex) {
  try {
    _model.state = collectState();
    _model.state = compressMemory(_model.state, blockIndex);
    currentActions += "\nОбъединены блоки (start=#%1)"_qs.arg(blockIndex);
    refresh();
  } catch (const OperationException &ex) {
    if (ex.what() == "SINGLE_BLOCK"s) {
      warning("Недостаточно блоков для слияния");
    } else {
      throw;
    }
  } catch (const std::exception &ex) {
    warning("Неизвестная ошибка: "s + ex.what());
  }
}

void MemoryTask::processActionDefragment() {
  try {
    _model.state = collectState();
    _model.state = defragmentMemory(_model.state);
    currentActions += "\nПамять дефрагментирована"_qs;
    refresh();
  } catch (const std::exception &ex) {
    warning("Неизвестная ошибка: "s + ex.what());
  }
}

void MemoryTask::updateCurrentRequest(int index) {
  if (index == -1) {
    return;
  }
  auto indexu = static_cast<std::size_t>(index);
  currentRequest = indexu;
  if (indexu == _model.task.completed() && !_model.task.done()) {
    unlockUi();
  } else {
    lockUi();
  }
  refresh();

  qDebug() << "currentRequest: " << currentRequest;
}

void MemoryTask::nextRequest() {
  _model.state = collectState();

  if (auto [ok, task] = _model.task.next(_model.state); ok) {
    _model.task = task;
    _model.state = _model.task.state();
    states[currentRequest] = _model.state;

    if (_model.task.done()) {
      QMessageBox::information(
          this, "Внимание", "Вы успешно выполнили данное задание");
      lockUi();
    }
    actions.push_back(currentActions);
    currentActions = "";
    updateHistoryView(_model.task, actions);

  } else {
    _model.task = task;
    warning("Заявка обработана неверно");
  }

  refresh();

  qDebug() << "currentRequest: " << currentRequest;
}

void MemoryTask::refresh() {
  auto request = currentRequest == _model.task.requests().size()
                     ? _model.task.requests().back()
                     : _model.task.requests().at(currentRequest);
  auto state = currentRequest == _model.task.completed()
                   ? _model.state
                   : states[currentRequest];
  auto strategy = _model.task.strategy();

  updateMainView(state, request);
  updateStrategyView(strategy->type);
  updateStatsView(_model.task.completed(),
                  _model.task.requests().size(),
                  _model.task.fails());
  updateCurrentActionsView(_model.task, currentActions);
}

void MemoryTask::lockUi() {
  ui->resetState->setDisabled(true);
  ui->acceptRequest->setDisabled(true);
  ui->listMemBlocks->setContextMenuPolicy(Qt::NoContextMenu);
  ui->listFreeBlocks->setDragEnabled(false);
}

void MemoryTask::unlockUi() {
  ui->resetState->setEnabled(true);
  ui->acceptRequest->setEnabled(true);
  ui->listMemBlocks->setContextMenuPolicy(Qt::CustomContextMenu);
  ui->listFreeBlocks->setDragEnabled(true);
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

void MemoryTask::updateMainView(MemoryState state, Request request) {
  auto [blocks, freeBlocks] = state;
  setMemoryBlocks(blocks);
  setFreeMemoryBlocks(freeBlocks);
  setRequest(request);

  int32_t available = 0;
  for (const auto &block : freeBlocks) {
    available += block.size();
  }
  ui->labelAvailableMemory->setText(
      "Всего доступно памяти: %1"_qs.arg(available));
}

void MemoryTask::updateStatsView(std::size_t count,
                                 std::size_t total,
                                 uint32_t fails) {
  ui->statsLabel->setText(
      "Обработано заявок: %1 из %2; ошибок: %3"_qs.arg(count).arg(total).arg(
          fails));
}

void MemoryTask::updateStrategyView(StrategyType type) {
  auto *label = ui->strategyLabel;

  if (type == StrategyType::FIRST_APPROPRIATE) {
    label->setText("Стратегия: первый подходящий");
  } else if (type == StrategyType::MOST_APPROPRIATE) {
    label->setText("Стратегия: наиболее подходящий");
  } else if (type == StrategyType::LEAST_APPROPRIATE) {
    label->setText("Стратегия: наименее подходящий");
  }
}

void MemoryTask::updateHistoryView(Utils::MemoryTask task,
                                   const std::vector<QString> &actions) {
  ui->historyList->clear();
  auto templ = QString("Заявка #%1");
  for (size_t i = 0; i < task.completed(); ++i) {
    auto *item = new QListWidgetItem();
    item->setText(templ.arg(i + 1));
    item->setToolTip(
        ACTIONS_TEMPL.arg(actions[i].isEmpty() ? NO_ACTIONS : actions[i]));
    ui->historyList->addItem(item);
  }
  if (!task.done()) {
    ui->historyList->addItem(templ.arg(ui->historyList->count() + 1));
  }
  ui->historyList->setCurrentRow(ui->historyList->count() - 1);
}

void MemoryTask::updateCurrentActionsView(Utils::MemoryTask task,
                                          const QString actions) {
  if (auto *item = ui->historyList->item(ui->historyList->count() - 1);
      item && !task.done()) {
    item->setToolTip(
        ACTIONS_TEMPL.arg(actions.isEmpty() ? NO_ACTIONS : actions));
  }
}

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
    list->addItem(new MemoryBlockItem(block, true));
  }
}

void MemoryTask::setRequest(const Request &request) {
  auto text = request.match(
      [](const CreateProcessReq &req) {
        return "Создан новый процесс PID = %1. "
               "Для размещения процесса в памяти (включая "
               "служебную информацию) требуется выделить "
               "%2 байт (%3 параграфов)"_qs.arg(req.pid())
                   .arg(req.bytes())
                   .arg(req.pages());
      },
      [](const TerminateProcessReq &req) {
        return "Процесс PID = %1 завершен"_qs.arg(req.pid());
      },
      [](const AllocateMemory &req) {
        return "Процесс PID = %1 выдал запрос на выделение ему "
               "%2 байт (%3 параграфов) оперативной памяти"_qs.arg(req.pid())
                   .arg(req.bytes())
                   .arg(req.pages());
      },
      [](const FreeMemory &req) {
        return "Процесс PID = %1 выдал запрос на освобождение "
               "блока памяти с адресом %2"_qs.arg(req.pid())
                   .arg(req.address());
      });
  ui->labelRequestDescr->setText(text);
}

void MemoryTask::warning(const std::string &message) {
  QMessageBox::warning(this, "Ошибка", QString::fromStdString(message));
}
