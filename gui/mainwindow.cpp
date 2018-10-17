#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "memorytask.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->clear();
    ui->tabWidget->addTab(new MemoryTask(this), "Менеджер памяти");
    ui->tabWidget->addTab(new MemoryTask(this), "Менеджер памяти");
    ui->tabWidget->addTab(new MemoryTask(this), "Менеджер памяти");
}

MainWindow::~MainWindow()
{
    delete ui;
}
