#include <algorithm>
#include <cstdint>
#include <limits>

#include <QDialogButtonBox>
#include <QIntValidator>
#include <QMessageBox>

#include <algo/processes/types.h>

#include "createprocessdialog.h"
#include "ui_createprocessdialog.h"

CreateProcessDialog::CreateProcessDialog(
    const CreateProcessDialog::ProcessesList &processes, QWidget *parent)
    : QDialog(parent), pids(processes.size(), -1),
      ui(new Ui::CreateProcessDialog) {
  ui->setupUi(this);
  ui->lineEditPID->setValidator(new QIntValidator(0, 255));
  ui->lineEditPPID->setValidator(new QIntValidator(-1, 255));
  ui->lineEditPriority->setValidator(new QIntValidator(0, 15));
  ui->lineEditBasePriority->setValidator(new QIntValidator(0, 15));
  ui->lineEditWorkTime->setValidator(
      new QIntValidator(0, std::numeric_limits<int>::max()));

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
          &CreateProcessDialog::tryAccept);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this,
          &CreateProcessDialog::reject);

  std::transform(processes.begin(), processes.end(), pids.begin(),
                 [](const auto &p) { return p.pid(); });
}

CreateProcessDialog::~CreateProcessDialog() { delete ui; }

void CreateProcessDialog::tryAccept() {
  bool valid = ui->lineEditPID->hasAcceptableInput()             //
               && ui->lineEditPPID->hasAcceptableInput()         //
               && ui->lineEditPriority->hasAcceptableInput()     //
               && ui->lineEditBasePriority->hasAcceptableInput() //
               && ui->lineEditWorkTime->hasAcceptableInput();
  if (!valid) {
    QMessageBox::critical(this, "Ошибка", "Поля заполнены неверно");
    return;
  }

  int32_t pid = ui->lineEditPID->text().toInt();
  int32_t ppid = ui->lineEditPPID->text().toInt();
  int32_t priority = ui->lineEditPriority->text().toInt();
  int32_t basePriority = ui->lineEditBasePriority->text().toInt();
  int32_t workTime = ui->lineEditWorkTime->text().toInt();

  if (std::find(pids.begin(), pids.end(), pid) != pids.end()) {
    QMessageBox::critical(this, "Ошибка", "Процесс с таким PID уже существует");
    return;
  }
  if (ppid != -1 && std::find(pids.begin(), pids.end(), ppid) == pids.end()) {
    QMessageBox::critical(this, "Ошибка",
                          "Родительский процесс с таким PID не существует");
    return;
  }
  if (priority < basePriority) {
    QMessageBox::critical(this, "Ошибка",
                          "Текущий приоритет не может быть меньше базового");
    return;
  }

  this->data = ProcessesManagement::Process{}
                   .pid(pid)
                   .ppid(ppid)
                   .priority(priority)
                   .basePriority(basePriority)
                   .workTime(workTime);
  this->accept();
}
