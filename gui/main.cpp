#include "mainwindow.h"
#include "presenters.h"
#include <QApplication>

using namespace Ui::Presenters;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    MainWindowPresenter presenter(&w);
    w.show();

    return a.exec();
}
