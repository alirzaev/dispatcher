#include <QApplication>
#include <QFontDatabase>
#include <QLibraryInfo>
#include <QTranslator>
#include <QInputDialog>
#include <QMessageBox>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setAttribute(Qt::AA_DisableWindowContextHelpButton);

  QTranslator translator;
  if (translator.load("qt_ru", ":/translations")) {
    a.installTranslator(&translator);
  }

  QFontDatabase::addApplicationFont(":/fonts/CascadiaMono.ttf");

  bool ok;
  QString student = "";
  while (true) {
    student =
        QInputDialog::getText(
            nullptr, "Ввод данных", "Фамилия И. О.", QLineEdit::Normal, "", &ok)
            .simplified();

    if (ok && student.isEmpty()) {
      QMessageBox::warning(nullptr, "Ошибка", "Введите свои Ф. И. О.");
    } else {
      break;
    }
  }

  if (!ok) {
    return 0;
  }

  MainWindow w(student);
  w.show();

  return a.exec();
}
