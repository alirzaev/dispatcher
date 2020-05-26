#include <map>
#include <utility>

#include <QAbstractItemView>
#include <QHeaderView>
#include <QIcon>
#include <QString>
#include <QTableWidgetItem>

#include <algo/processes/types.h>

#include <qtutils/literals.h>

#include "processestablewidget.h"

using namespace ProcessesManagement;
using namespace QtUtils::Literals;

using TextItem = QTableWidgetItem;

extern void initResources();

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
  initResources();

  setColumnCount(8);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setEditTriggers({QTableWidget::NoEditTriggers});

  setHorizontalHeaderLabels({"PID"_qs,
                             "PPID"_qs,
                             "Состояние"_qs,
                             "Приториет"_qs,
                             "Б. приоритет"_qs,
                             "t"_qs,
                             "T"_qs,
                             "T-t"_qs});
  setSortingEnabled(true);
  resizeColumnsToContents();
  verticalHeader()->setVisible(false);
}

void ProcessesTableWidget::setProcesses(const ProcessesList &processes) {
  setRowCount(static_cast<int>(processes.size()));
  setSortingEnabled(false);

  std::map<ProcState, std::pair<QString, QString>> stateMap = {
      {ProcState::ACTIVE, {":/g/images/yellow_circle.png"_qs, "Q"}},
      {ProcState::WAITING, {":/g/images/red_circle.png"_qs, "W"}},
      {ProcState::EXECUTING, {":/g/images/green_circle.png"_qs, "E"}}};

  for (size_t i = 0; i < processes.size(); ++i) {
    const auto &p = processes[i];
    auto row = static_cast<int>(i);

    auto [stateIcon, stateText] = stateMap[p.state()];

    setItem(row, 0, new NumberItem(p.pid()));
    setItem(row, 1, new NumberItem(p.ppid()));
    setItem(row, 2, new TextItem(QIcon(stateIcon), stateText));
    setItem(row, 3, new NumberItem(static_cast<int32_t>(p.priority())));
    setItem(row, 4, new NumberItem(static_cast<int32_t>(p.basePriority())));
    setItem(row, 5, new NumberItem(p.timer()));
    setItem(row, 6, new NumberItem(p.workTime()));
    setItem(row, 7, new NumberItem(p.workTime() - p.timer()));
  }

  setSortingEnabled(true);
}

ProcessesTableWidget::~ProcessesTableWidget() {}
