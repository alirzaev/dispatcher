#include <QApplication>
#include <QFontDatabase>
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

  QFontDatabase::addApplicationFont(":/fonts/CascadiaMono.ttf");

  MainWindow w;
  w.show();

  return a.exec();
}
