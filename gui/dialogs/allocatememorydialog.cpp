#include <utility>

#include <QDialogButtonBox>
#include <QIntValidator>
#include <QMessageBox>

#include <tl/optional.hpp>

#include <qtutils/literals.h>

#include "allocatememorydialog.h"
#include "ui_allocatememorydialog.h"

using namespace QtUtils::Literals;

AllocateMemoryDialog::AllocateMemoryDialog(QWidget *parent,
                                           int32_t availablePages)
    : QDialog(parent), ui(new Ui::AllocateMemoryDialog),
      availablePages(availablePages) {
  ui->setupUi(this);
  ui->PIDEdit->setValidator(new QIntValidator(0, 255));
  ui->sizeEdit->setValidator(new QIntValidator(1, 256));

  connect(ui->buttonBox,
          &QDialogButtonBox::accepted,
          this,
          &AllocateMemoryDialog::tryAccept);
  connect(ui->buttonBox,
          &QDialogButtonBox::rejected,
          this,
          &AllocateMemoryDialog::reject);
}

AllocateMemoryDialog::~AllocateMemoryDialog() { delete ui; }

tl::optional<std::pair<int32_t, int32_t>>
AllocateMemoryDialog::getMemoryBlockInfo(QWidget *parent,
                                         int32_t availablePages) {
  auto dialog = AllocateMemoryDialog(parent, availablePages);
  if (dialog.exec() == QDialog::Accepted) {
    return dialog.info;
  } else {
    return tl::nullopt;
  }
}

void AllocateMemoryDialog::tryAccept() {
  bool valid = ui->PIDEdit->hasAcceptableInput() //
               && ui->sizeEdit->hasAcceptableInput();
  if (!valid) {
    QMessageBox::warning(this, "Ошибка", "Поля заполнены неверно");
    return;
  }

  int32_t pid = ui->PIDEdit->text().toInt();
  int32_t size = ui->sizeEdit->text().toInt();

  if (size > availablePages) {
    QMessageBox::warning(
        this,
        "Ошибка",
        "Максимальное количество параграфов для выделения: %1"_qs.arg(
            availablePages));
    return;
  }

  this->info = {pid, size};
  this->accept();
}
