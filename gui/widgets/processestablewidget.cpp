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

using TextItem = QTableWidgetItem;

class NumberItem : public QTableWidgetItem {
public:
  NumberItem() : NumberItem(0) {}

  NumberItem(int32_t number) : QTableWidgetItem("%1"_qs.arg(number)) {}

  virtual bool operator<(const QTableWidgetItem &rhs) const {
    bool okLeft, okRight;
    int32_t left, right;
    left = text().toInt(&okLeft);
    right = rhs.text().toInt(&okRight);

    if (okLeft && okRight) {
      return left < right;
    } else {
      return QTableWidgetItem::operator<(rhs);
    }
  }
};

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
  setSortingEnabled(false);

  std::map<ProcState, QString> stateMap = {{ProcState::ACTIVE, "A"_qs},
                                           {ProcState::WAITING, "W"_qs},
                                           {ProcState::EXECUTING, "E"_qs}};

  for (size_t i = 0; i < processes.size(); ++i) {
    const auto &p = processes[i];
    auto row = static_cast<int>(i);

    setItem(row, 0, new NumberItem(p.pid()));
    setItem(row, 1, new NumberItem(p.ppid()));
    setItem(row, 2, new TextItem(stateMap[p.state()]));
    setItem(row, 3, new NumberItem(p.priority()));
    setItem(row, 4, new NumberItem(p.basePriority()));
    setItem(row, 5, new NumberItem(p.timer()));
    setItem(row, 6, new NumberItem(p.workTime()));
    setItem(row, 7, new NumberItem(p.workTime() - p.timer()));
  }

  setSortingEnabled(true);
}

ProcessesTableWidget::~ProcessesTableWidget() {}
