#include "allocatememorydialog.h"
#include "ui_allocatememorydialog.h"

#include <QDialogButtonBox>
#include <QIntValidator>

AllocateMemoryDialog::AllocateMemoryDialog(QWidget *parent, int32_t maxSize) :
    QDialog(parent),
    ui(new Ui::AllocateMemoryDialog),
    maxSize(maxSize)
{
    ui->setupUi(this);
    ui->PIDEdit->setValidator(new QIntValidator(0, 255));
    ui->sizeEdit->setValidator(new QIntValidator(1, maxSize));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AllocateMemoryDialog::tryAccept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AllocateMemoryDialog::reject);
}

AllocateMemoryDialog::~AllocateMemoryDialog()
{
    delete ui;
}

void AllocateMemoryDialog::tryAccept()
{
    if (!ui->PIDEdit->text().isEmpty() && !ui->sizeEdit->text().isEmpty())
    {
        this->data = {
            ui->PIDEdit->text().toInt(),
            ui->sizeEdit->text().toInt()
        };
        this->accept();
    }
}
