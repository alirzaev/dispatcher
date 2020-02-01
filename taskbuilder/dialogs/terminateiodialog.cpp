#include <utility>

#include <QDialogButtonBox>
#include <QIntValidator>
#include <QMessageBox>

#include <tl/optional.hpp>

#include <qtutils/literals.h>

#include "terminateiodialog.h"
#include "ui_terminateiodialog.h"

TerminateIoDialog::TerminateIoDialog(QWidget *parent, bool withAugment)
    : QDialog(parent), ui(new Ui::TerminateIoDialog) {
  ui->setupUi(this);
  ui->PIDEdit->setValidator(new QIntValidator(0, 255));
  ui->augmentEdit->setValidator(new QIntValidator(0, 15));

  if (!withAugment) {
    ui->augmentEdit->setText("1");
    ui->augmentEdit->setDisabled(true);
  }

  connect(ui->buttonBox,
          &QDialogButtonBox::accepted,
          this,
          &TerminateIoDialog::tryAccept);
  connect(ui->buttonBox,
          &QDialogButtonBox::rejected,
          this,
          &TerminateIoDialog::reject);
}

void TerminateIoDialog::tryAccept() {
  bool valid = ui->PIDEdit->hasAcceptableInput() //
               && ui->augmentEdit->hasAcceptableInput();
  if (!valid) {
    QMessageBox::warning(this, "Ошибка", "Поля заполнены неверно");
    return;
  }

  int32_t pid = ui->PIDEdit->text().toInt();
  int32_t augment = ui->augmentEdit->text().toInt();

  this->info = {pid, static_cast<std::size_t>(augment)};
  this->accept();
}

TerminateIoDialog::~TerminateIoDialog() { delete ui; }

tl::optional<std::pair<int32_t, size_t>>
TerminateIoDialog::getTerminateIoInfo(QWidget *parent, bool withAugment) {
  auto dialog = TerminateIoDialog(parent, withAugment);
  if (dialog.exec() == QDialog::Accepted) {
    return dialog.info;
  } else {
    return tl::nullopt;
  }
}
