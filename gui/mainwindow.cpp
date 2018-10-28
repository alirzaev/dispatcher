#include <QAction>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "memorytask.h"

using namespace Views;
using namespace Utils;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    Views::MainWindowView(),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->clear();

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
        if (!fileName.endsWith(".json"))
        {
            fileName.append(".json");
        }
        saveTaskListener(fileName.toStdString());
    }
}

std::vector<Views::TaskView>
MainWindow::createTaskViews(const std::vector<Tasks::Task>& tasks)
{
    std::vector<Views::TaskView> views;

    auto* tabs = ui->tabWidget;
    tabs->clear();
    for (const auto& task : tasks)
    {
        if (auto* p = std::get_if<Tasks::MemoryTask>(&task))
        {
            auto* taskView = new MemoryTask(this);
            tabs->addTab(taskView, "Диспетчеризация памяти");
            views.push_back(taskView);
        }
    }

    return views;
}
