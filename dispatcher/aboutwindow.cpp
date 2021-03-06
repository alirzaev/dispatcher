#include <qtutils/literals.h>

#include <config.h>

#include "aboutwindow.h"
#include "ui_aboutwindow.h"

using namespace QtUtils::Literals;

AboutWindow::AboutWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::AboutWindow) {
  ui->setupUi(this);

  ui->labelVersion->setText("Версия: %1.%2.%3"_qs.arg(DISPATCHER_MAJOR)
                                .arg(DISPATCHER_MINOR)
                                .arg(DISPATCHER_PATCH));
  ui->labelGitRevision->setText("Ревизия: %1"_qs.arg(GIT_REV));
}

AboutWindow::~AboutWindow() { delete ui; }
