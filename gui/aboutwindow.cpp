#include "literals.h"

#include "aboutwindow.h"
#include "ui_aboutwindow.h"

using namespace Utils::Literals;

AboutWindow::AboutWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::AboutWindow) {
  ui->setupUi(this);

  ui->labelTitle->setText(
      "Модель диспетчера задач ОС %1.%2.%3"_qs.arg(DISPATCHER_MAJOR)
          .arg(DISPATCHER_MINOR)
          .arg(DISPATCHER_PATCH));
}

AboutWindow::~AboutWindow() { delete ui; }
