#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setAttribute(Qt::AA_DisableWindowContextHelpButton);

  QTranslator translator;
  if (translator.load("qt_ru", ":/translations")) {
    a.installTranslator(&translator);
  }

  MainWindow w;
  w.show();

  return a.exec();
}
