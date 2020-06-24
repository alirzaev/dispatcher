#pragma once

#include <cstdint>

#include <QDialog>
#include <QWidget>
#include <QFlags>

#include <tl/optional.hpp>

#include <algo/processes/types.h>

namespace Ui {
class CreateProcessDialog;
}

class CreateProcessDialog : public QDialog {
  Q_OBJECT

public:
  enum class EditableField : uint32_t {
    Pid = 0x1,
    Ppid = 0x2,
    Priority = 0x4,
    BasePriority = 0x8,
    WorkTime = 0x10
  };
  Q_DECLARE_FLAGS(EditableFields, EditableField)

  using ProcessesList =
      decltype(ProcessesManagement::ProcessesState::processes);

  static tl::optional<ProcessesManagement::Process>
  getProcess(QWidget *parent,
             const ProcessesList &processes,
             const QFlags<EditableField> &editableFields,
             bool basicValidation = false);

  ~CreateProcessDialog() override;

private:
  explicit CreateProcessDialog(const ProcessesList &processes,
                               const QFlags<EditableField> &editableFields,
                               bool basicValidation = false,
                               QWidget *parent = nullptr);
  ProcessesList processes;

  ProcessesManagement::Process data;

  bool basicValidation;

  Ui::CreateProcessDialog *ui;

  void tryAccept();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CreateProcessDialog::EditableFields)
