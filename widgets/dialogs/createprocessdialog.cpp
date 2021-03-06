#include <cstddef>
#include <cstdint>

#include <QDialogButtonBox>
#include <QFlags>
#include <QIntValidator>
#include <QMessageBox>

#include <tl/optional.hpp>

#include <algo/processes/helpers.h>
#include <algo/processes/types.h>

#include "createprocessdialog.h"
#include "ui_createprocessdialog.h"

CreateProcessDialog::CreateProcessDialog(
    const ProcessesList &processes,
    const QFlags<EditableField> &editableFields,
    bool basicValidation,
    QWidget *parent)
    : QDialog(parent), processes(processes), basicValidation(basicValidation),
      ui(new Ui::CreateProcessDialog) {
  ui->setupUi(this);
  ui->lineEditPID->setValidator(new QIntValidator(0, 255));
  ui->lineEditPPID->setValidator(new QIntValidator(-1, 255));
  ui->lineEditPriority->setValidator(new QIntValidator(0, 15));
  ui->lineEditBasePriority->setValidator(new QIntValidator(0, 15));
  ui->lineEditWorkTime->setValidator(
      new QIntValidator(0, std::numeric_limits<int>::max()));

  ui->lineEditPID->setEnabled(editableFields.testFlag(EditableField::Pid));
  ui->lineEditPPID->setEnabled(editableFields.testFlag(EditableField::Ppid));
  ui->lineEditPriority->setEnabled(
      editableFields.testFlag(EditableField::Priority));
  ui->lineEditBasePriority->setEnabled(
      editableFields.testFlag(EditableField::BasePriority));
  ui->lineEditWorkTime->setEnabled(
      editableFields.testFlag(EditableField::WorkTime));

  connect(ui->buttonBox,
          &QDialogButtonBox::accepted,
          this,
          &CreateProcessDialog::tryAccept);
  connect(ui->buttonBox,
          &QDialogButtonBox::rejected,
          this,
          &CreateProcessDialog::reject);
}

tl::optional<ProcessesManagement::Process> CreateProcessDialog::getProcess(
    QWidget *parent,
    const CreateProcessDialog::ProcessesList &processes,
    const QFlags<CreateProcessDialog::EditableField> &editableFields,
    bool basicValidation) {
  auto dialog =
      CreateProcessDialog(processes, editableFields, basicValidation, parent);
  if (dialog.exec() == QDialog::Accepted) {
    return dialog.data;
  } else {
    return tl::nullopt;
  }
}

CreateProcessDialog::~CreateProcessDialog() { delete ui; }

void CreateProcessDialog::tryAccept() {
  using ProcessesManagement::getIndexByPid;

  bool valid = ui->lineEditPID->hasAcceptableInput()             //
               && ui->lineEditPPID->hasAcceptableInput()         //
               && ui->lineEditPriority->hasAcceptableInput()     //
               && ui->lineEditBasePriority->hasAcceptableInput() //
               && ui->lineEditWorkTime->hasAcceptableInput();
  if (!valid) {
    QMessageBox::warning(this, "Ошибка", "Поля заполнены неверно");
    return;
  }

  int32_t pid = ui->lineEditPID->text().toInt();
  int32_t ppid = ui->lineEditPPID->text().toInt();
  int32_t priority = ui->lineEditPriority->text().toInt();
  int32_t basePriority = ui->lineEditBasePriority->text().toInt();
  int32_t workTime = ui->lineEditWorkTime->text().toInt();

  if (!basicValidation) {
    if (getIndexByPid(processes, pid)) {
      QMessageBox::warning(
          this, "Ошибка", "Процесс с таким PID уже существует");
      return;
    }
    if (ppid != -1 && !getIndexByPid(processes, ppid)) {
      QMessageBox::warning(
          this, "Ошибка", "Родительский процесс с таким PID не существует");
      return;
    }
  }

  if (priority < basePriority) {
    QMessageBox::warning(
        this, "Ошибка", "Текущий приоритет не может быть меньше базового");
    return;
  }

  this->data = ProcessesManagement::Process{}
                   .pid(pid)
                   .ppid(ppid)
                   .priority(static_cast<size_t>(priority))
                   .basePriority(static_cast<size_t>(basePriority))
                   .workTime(workTime);
  this->accept();
}
