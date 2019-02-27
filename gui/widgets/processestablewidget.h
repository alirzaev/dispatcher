#pragma once

#include <vector>

#include <QTableWidget>
#include <QWidget>

#include <algo/processes/types.h>

class ProcessesTableWidget : public QTableWidget {
private:
  using ProcessesList =
      decltype(ProcessesManagement::ProcessesState::processes);

public:
  explicit ProcessesTableWidget(QWidget *parent = nullptr);

  void setProcesses(const ProcessesList &processes);

  ~ProcessesTableWidget() override;
};
