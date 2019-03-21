#include <map>
#include <vector>

#include <QAbstractItemView>
#include <QHeaderView>
#include <QString>
#include <QTableWidgetItem>

#include <algo/processes/types.h>

#include "../literals.h"

#include "processestablewidget.h"

using namespace ProcessesManagement;
using namespace Utils::Literals;

using Item = QTableWidgetItem;

ProcessesTableWidget::ProcessesTableWidget(QWidget *parent)
    : QTableWidget(parent) {
  setColumnCount(8);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setEditTriggers({QTableWidget::NoEditTriggers});

  setHorizontalHeaderLabels({"PID"_qs, "PPID"_qs, "Состояние"_qs,
                             "Приториет"_qs, "Б. приоритет"_qs, "t"_qs, "T"_qs,
                             "T-t"_qs});
  setSortingEnabled(true);
  resizeColumnsToContents();
  verticalHeader()->setVisible(false);
}

void ProcessesTableWidget::setProcesses(const ProcessesList &processes) {
  setRowCount(static_cast<int>(processes.size()));

  std::map<ProcState, QString> stateMap = {{ProcState::ACTIVE, "A"_qs},
                                           {ProcState::WAITING, "W"_qs},
                                           {ProcState::EXECUTING, "E"_qs}};

  for (size_t i = 0; i < processes.size(); ++i) {
    const auto &p = processes[i];
    auto row = static_cast<int>(i);

    setItem(row, 0, new Item("%1"_qs.arg(p.pid())));
    setItem(row, 1, new Item("%1"_qs.arg(p.ppid())));
    setItem(row, 2, new Item(stateMap[p.state()]));
    setItem(row, 3, new Item("%1"_qs.arg(p.priority())));
    setItem(row, 4, new Item("%1"_qs.arg(p.basePriority())));
    setItem(row, 5, new Item("%1"_qs.arg(p.timer())));
    setItem(row, 6, new Item("%1"_qs.arg(p.workTime())));
    setItem(row, 7, new Item("%1"_qs.arg(p.workTime() - p.timer())));
  }
}

ProcessesTableWidget::~ProcessesTableWidget() {}
