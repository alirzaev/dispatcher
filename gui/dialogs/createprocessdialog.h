#pragma once

#include <cstdint>
#include <vector>

#include <QDialog>

#include <algo/processes/types.h>

namespace Ui {
class CreateProcessDialog;
}

class CreateProcessDialog : public QDialog {
  Q_OBJECT

public:
  using ProcessesList =
      decltype(ProcessesManagement::ProcessesState::processes);

  ProcessesManagement::Process data;

  explicit CreateProcessDialog(const ProcessesList &processes,
                               QWidget *parent = nullptr);

  ~CreateProcessDialog() override;

private:
  ProcessesList processes;

  Ui::CreateProcessDialog *ui;

  void tryAccept();
};
