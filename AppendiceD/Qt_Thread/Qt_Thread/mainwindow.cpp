#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_thObj = new ThreadedClass();

    connect(ui->m_startStop, &QPushButton::clicked, m_thObj, &ThreadedClass::StartStopThread);
    connect(m_thObj, &ThreadedClass::SendString, this, &MainWindow::ReceiveString);
    connect(m_thObj, &ThreadedClass::SendRunningStatus, this ,&MainWindow::HandleRunningStatus);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ReceiveString(QString p_item)
{
    ui->m_listView->addItem(p_item);
    ui->m_listView->scrollToBottom();
}

void MainWindow::HandleRunningStatus(bool p_running)
{
    if (p_running)
    {
        ui->m_startStop->setText("Stop");
    }
    else
    {
        ui->m_startStop->setText("Start");
    }
}
