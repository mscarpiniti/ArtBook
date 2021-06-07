#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QIcon play, rec;
    QPixmap qpm;

    //Inizializzazione dei pulsanti e delle icone
    if (qpm.load(":/icons/play.png")) {
        play.addPixmap(qpm);
        ui->play->setIcon(play);
        ui->play->setIconSize(QSize(100,100));
    } else {
        ui->play->setText("Play");
    }

    if (qpm.load(":/icons/record.png")) {
        rec.addPixmap(qpm);
        ui->record->setIcon(rec);
        ui->record->setIconSize(QSize(100,100));
    } else {
        ui->record->setText("Start\nRecord");
    }

    m_theController = controller::GetInstance();

    m_recAnim = new QMovie(":/anims/recording.gif");
    m_recAnim->setScaledSize(QSize(ui->statusbar->height() - 5, ui->statusbar->height() - 5));
    m_playAnim = new QMovie(":/anims/playing.gif");
    m_playAnim->setScaledSize(QSize(ui->statusbar->height() + 15, ui->statusbar->height() + 15));

    m_recordLabel = new QLabel(this);
    m_recordLabel->setMovie(m_recAnim);
    ui->statusbar->addWidget(m_recordLabel);
    m_recAnim->start();
    m_recAnim->stop();

    m_playProgress = new QProgressBar(this);
    ui->statusbar->addWidget(m_playProgress);

    connect(ui->play,&QPushButton::clicked,this,&MainWindow::PlayBtnPressed);
    connect(ui->record,&QPushButton::clicked,this,&MainWindow::RecordBtnPressed);
    connect(m_theController,&controller::NotifyStatusChange,this,&MainWindow::HandleStatusChange);
    connect(m_theController,&controller::UpdatePlayProgress,this,&MainWindow::SetProgress);
    connect(m_theController,&controller::OpenFile,this,&MainWindow::OpenFile);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::SetIcon(QPushButton* tgt, QString url, QString bk)
{
    QIcon icon;
    QPixmap qpm;

    if (qpm.load(url)) {
        icon.addPixmap(qpm);
        tgt->setIcon(icon);
    } else {
        tgt->setText(bk);
    }
}


void MainWindow::HandleStatusChange(State newState)
{
    switch(newState) {
    case Stopped:
       {
            SetIcon(ui->record,":/icons/record.png","Record");
            SetIcon(ui->play,":/icons/play.png","Play");
            ui->play->setEnabled(true);
            ui->record->setEnabled(true);
            m_playProgress->setEnabled(false);
            m_recAnim->stop();

        }
        break;
    case Playing:
        {
            SetIcon(ui->play,":/icons/stop.png","Play");
            ui->record->setEnabled(false);
            m_playProgress->setRange(0,1000);
            m_playProgress->setEnabled(true);
        }
        break;
    case Recording:
        {
            SetIcon(ui->record,":/icons/stop.png","Record");
            ui->play->setEnabled(false);
            m_recAnim->start();
        }
        break;
    }
}


void MainWindow::SetProgress(int value) {
   m_playProgress->setValue(value);
}


QString MainWindow::OpenFile(openMode mode)
{
    QString fileName;
    switch(mode) {
    case Save:
        fileName = QFileDialog::getSaveFileName(this,tr("Save wav file."),
                                                QDir::homePath(),
                                                "Audio Files (*.wav)");
        break;
    case Open:
        fileName = QFileDialog::getOpenFileName(this,tr("Open wav file."),
                                                QDir::homePath(),
                                                "Audio Files (*.wav)");
        break;

    }

    return fileName;
}

void MainWindow::PlayBtnPressed()
{
    if (m_theController->GetStatus() == Stopped)
        m_theController->Play();
    else m_theController->Stop();
}

void MainWindow::RecordBtnPressed()
{
    if (m_theController->GetStatus() == Stopped)
        m_theController->Rec();
    else m_theController->Stop();
}

