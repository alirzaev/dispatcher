#pragma once

#include <vector>

#include <QObject>
#include <QTableWidget>
#include <QWidget>

#include <algo/processes/types.h>

class ProcessesTableWidget : public QTableWidget {
  Q_OBJECT
private:
  using ProcessesList =
      decltype(ProcessesManagement::ProcessesState::processes);

public:
  explicit ProcessesTableWidget(QWidget *parent = nullptr);

  void setProcesses(const ProcessesList &processes);

  ~ProcessesTableWidget() override;
};
