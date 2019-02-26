#include <QDialogButtonBox>
#include <QIntValidator>

#include "allocatememorydialog.h"
#include "ui_allocatememorydialog.h"

AllocateMemoryDialog::AllocateMemoryDialog(QWidget *parent, int32_t maxSize)
    : QDialog(parent), ui(new Ui::AllocateMemoryDialog), maxSize(maxSize) {
  ui->setupUi(this);
  ui->PIDEdit->setValidator(new QIntValidator(0, 255));
  ui->sizeEdit->setValidator(new QIntValidator(1, maxSize));

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
          &AllocateMemoryDialog::tryAccept);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this,
          &AllocateMemoryDialog::reject);
}

AllocateMemoryDialog::~AllocateMemoryDialog() { delete ui; }

void AllocateMemoryDialog::tryAccept() {
  bool pidOk, sizeOk;
  int32_t pid = ui->PIDEdit->text().toInt(&pidOk, 10);
  int32_t size = ui->sizeEdit->text().toInt(&sizeOk, 10);
  if (pidOk && sizeOk && (pid > -1 && pid < 256) &&
      (size > 0 && size <= maxSize)) {
    this->data = {pid, size};
    this->accept();
  }
}
