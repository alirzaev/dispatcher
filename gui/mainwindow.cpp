#include <QAction>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "memorytask.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    Views::MainWindowView(),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->clear();
    ui->tabWidget->addTab(new MemoryTask(this), "Менеджер памяти");
    ui->tabWidget->addTab(new MemoryTask(this), "Менеджер памяти");
    ui->tabWidget->addTab(new MemoryTask(this), "Менеджер памяти");

    connect(ui->actionOpenTask, &QAction::triggered, this, &MainWindow::openTaskDialog);
    connect(ui->actionSaveTask, &QAction::triggered, this, &MainWindow::saveTaskDialog);
    connect(ui->actionQuit, &QAction::triggered, this, &QMainWindow::close);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onOpenTaskListener(OnOpenListener listener)
{
    openTaskListener = listener;
}

void MainWindow::onSaveTaskListener(OnSaveListener listener)
{
    saveTaskListener = listener;
}

void MainWindow::openTaskDialog()
{
    auto fileName = QFileDialog::getOpenFileName(this, "Открыть файл задания", "", "JSON (*.json)");
    if (!fileName.isEmpty() && openTaskListener)
    {
        openTaskListener(fileName.toStdString());
    }
}

void MainWindow::saveTaskDialog()
{
    auto fileName = QFileDialog::getSaveFileName(this, "Сохранить задание в файл", "", "JSON (*.json)");
    if (!fileName.isEmpty() && saveTaskListener)
    {
        saveTaskListener(fileName.toStdString());
    }
}
