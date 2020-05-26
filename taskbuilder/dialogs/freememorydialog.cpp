#include <QDialogButtonBox>
#include <QIntValidator>
#include <QMessageBox>

#include <tl/optional.hpp>

#include "freememorydialog.h"
#include "ui_freememorydialog.h"

FreeMemoryDialog::FreeMemoryDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::FreeMemoryDialog) {
  ui->setupUi(this);
  ui->PIDEdit->setValidator(new QIntValidator(0, 255));
  ui->addressEdit->setValidator(new QIntValidator(0, 255));

  connect(ui->buttonBox,
          &QDialogButtonBox::accepted,
          this,
          &FreeMemoryDialog::tryAccept);
  connect(ui->buttonBox,
          &QDialogButtonBox::rejected,
          this,
          &FreeMemoryDialog::reject);
}

void FreeMemoryDialog::tryAccept() {
  bool valid = ui->PIDEdit->hasAcceptableInput() //
               && ui->addressEdit->hasAcceptableInput();
  if (!valid) {
    QMessageBox::warning(this, "Ошибка", "Поля заполнены неверно");
    return;
  }

  int32_t pid = ui->PIDEdit->text().toInt();
  int32_t address = ui->addressEdit->text().toInt();

  this->info = {pid, address};
  this->accept();
}

FreeMemoryDialog::~FreeMemoryDialog() { delete ui; }

tl::optional<std::pair<int32_t, int32_t>>
FreeMemoryDialog::getFreeMemoryInfo(QWidget *parent) {
  auto dialog = FreeMemoryDialog(parent);
  if (dialog.exec() == QDialog::Accepted) {
    return dialog.info;
  } else {
    return tl::nullopt;
  }
}
