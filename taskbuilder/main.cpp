#include <iostream>

#include <QApplication>
#include <QEventLoop>

#include "dialogs/tasktypechooser.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setAttribute(Qt::AA_DisableWindowContextHelpButton);

  MainWindow w;
  w.show();

  return a.exec();
}
