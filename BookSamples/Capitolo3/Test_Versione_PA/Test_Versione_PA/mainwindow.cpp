#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

#include <string>
#include <QString>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetVersion(int val)
{
    ui->label_version->setText("Versione di PortAudio: " + QString::number(val));
}

void MainWindow::SetInfo(QString *val)
{
    ui->label_info->setText("Info versione: " + QString(*val));
}
