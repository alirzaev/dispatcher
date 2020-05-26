#include <QDialogButtonBox>
#include <QIntValidator>
#include <QMessageBox>

#include <tl/optional.hpp>

#include "pidinputdialog.h"
#include "ui_pidinputdialog.h"

PidInputDialog::PidInputDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::PidInputDialog) {
  ui->setupUi(this);
  ui->PIDEdit->setValidator(new QIntValidator(0, 255));

  connect(ui->buttonBox,
          &QDialogButtonBox::accepted,
          this,
          &PidInputDialog::tryAccept);
  connect(ui->buttonBox,
          &QDialogButtonBox::rejected,
          this,
          &PidInputDialog::reject);
}

void PidInputDialog::tryAccept() {
  if (!ui->PIDEdit->hasAcceptableInput()) {
    QMessageBox::warning(this, "Ошибка", "Поле заполнены неверно");
    return;
  }

  int32_t pid = ui->PIDEdit->text().toInt();

  this->info = pid;
  this->accept();
}

PidInputDialog::~PidInputDialog() { delete ui; }

tl::optional<int32_t> PidInputDialog::getPid(QWidget *parent) {
  auto dialog = PidInputDialog(parent);
  if (dialog.exec() == QDialog::Accepted) {
    return dialog.info;
  } else {
    return tl::nullopt;
  }
}
