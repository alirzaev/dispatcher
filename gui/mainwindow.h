#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "views.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public Ui::Views::MainWindowView
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void onOpenTaskListener(OnOpenListener listener) override;

    void onSaveTaskListener(OnSaveListener listener) override;

    ~MainWindow();

private:
    Ui::MainWindow *ui;

    OnOpenListener openTaskListener;

    OnSaveListener saveTaskListener;

    void openTaskDialog();

    void saveTaskDialog();
};

#endif // MAINWINDOW_H
