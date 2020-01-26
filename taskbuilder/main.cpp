#include <iostream>

#include <QApplication>
#include <QEventLoop>

#include "dialogs/tasktypechooser.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
