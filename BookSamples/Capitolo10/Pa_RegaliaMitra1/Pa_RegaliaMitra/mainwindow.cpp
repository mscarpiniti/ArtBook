#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QString>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_streamController = StreamController::GetInstance();
    m_streamController->InitController();

    m_filterParamsLocal = new RegMitraParameters();
    m_inputParamsLocal = new InputParameters();


    //Connessione di tutti i controlli ai rispettivi metodi che ne gestiscono le occorrenze
    connect(ui->m_start,&QPushButton::clicked,this,&MainWindow::StartStopStream);
    connect(m_streamController,&StreamController::NewStreamerStatus,this,&MainWindow::OnStreamingStatusChange);

    connect(ui->m_FilterEnable,&QCheckBox::stateChanged,this,&MainWindow::OnFilterChangeAbilitation);
    connect(ui->m_filterCutoffDial,&QDial::valueChanged,this,&MainWindow::OnFilterChangeCutoff);
    connect(ui->m_filterGainDial,&QDial::valueChanged,this,&MainWindow::OnFilterChangeLevel);

    //Controlli relativi all'input
    connect(ui->m_inputLevelDial,&QDial::valueChanged,this,&MainWindow::OnInputChangeLevel);

    //Inizializzazione dell'interfaccia grafica ai valori attuati
    ui->m_filterCutoffDial->setValue(m_filterParamsLocal->CutoffFrequency);
    ui->m_filterGainDial->setValue(m_filterParamsLocal->Gain * 100);
    ui->m_FilterEnable->setChecked(m_filterParamsLocal->Active);
    ui->m_filterGainLabel->setText(QString::number(20 * log10(m_filterParamsLocal->Gain),'g',2) + " [dB]");
    ui->m_inputLevelDial->setValue(m_inputParamsLocal->Gain * 100);
    ui->m_inputLevelLabel->setText(QString::number(20 * log10(m_inputParamsLocal->Gain),'g',2) + " [dB]");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_streamController->ReleaseResources();
    event->accept();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnStreamingStatusChange(StreamStatus p_status)
{
    switch(p_status)
    {
    case Playing:
        ui->m_start->setText("Stop");
        break;
    case Stopped:
    case Paused:
        ui->m_start->setText("Start");
        break;
    default:
        break;
    }
}

void MainWindow::StartStopStream()
{
    if (m_streamController->GetStreamStatus() == Playing)
    {
        m_streamController->StopStreaming();
    }
    else
    {
        m_streamController->StartStreaming();
    }
}

QString MainWindow::HertzToString(double p_freq)
{
    QString l_res;
    if (p_freq < 1000)
    {
        l_res = QString::number(p_freq) + " Hz";
    } else
    {
        float l_khz = p_freq / 1000;
        l_res = QString::number(l_khz) + " KHz";
    }

    return l_res;
}


//Filtro
void MainWindow::OnFilterChangeAbilitation(int p_state)
{
    m_filterParamsLocal->Active = p_state == Qt::Checked ? true : false;
    m_streamController->UpdateFilterParams(m_filterParamsLocal);
}

void MainWindow::OnFilterChangeCutoff(double p_freq)
{
    m_filterParamsLocal->CutoffFrequency = p_freq;
    m_streamController->UpdateFilterParams(m_filterParamsLocal);
    ui->m_FilterCutoffLabel->setText(HertzToString(p_freq));
}

void MainWindow::OnFilterChangeLevel(double p_level)
{
    m_filterParamsLocal->Gain = p_level/100;
    m_streamController->UpdateFilterParams(m_filterParamsLocal);
    ui->m_filterGainLabel->setText(QString::number(20 * log10(p_level/100)) + " [dB]");
}


//Input
void MainWindow::OnInputChangeLevel(double p_level)
{
    ui->m_inputLevelLabel->setText(QString::number(20 * log10(p_level/100),'g',2) + " [dB]");
    m_inputParamsLocal->Gain = p_level/100;
    m_streamController->UpdateInputParams(m_inputParamsLocal);
}


