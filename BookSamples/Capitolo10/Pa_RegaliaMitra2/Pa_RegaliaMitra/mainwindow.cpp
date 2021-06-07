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
    m_leftOscParamsLocal = new OscillatorParameters();
    m_rightOscParamsLocal = new OscillatorParameters();
    m_inputParamsLocal = new InputParameters();


    //Connessione di tutti i controlli ai rispettivi metodi che ne gestiscono le occorrenze
    connect(ui->m_start,&QPushButton::clicked,this,&MainWindow::StartStopStream);
    connect(m_streamController,&StreamController::NewStreamerStatus,this,&MainWindow::OnStreamingStatusChange);

    connect(ui->m_FilterEnable,&QCheckBox::stateChanged,this,&MainWindow::OnFilterChangeAbilitation);
    connect(ui->m_filterCutoffDial,&QDial::valueChanged,this,&MainWindow::OnFilterChangeCutoff);
    connect(ui->m_filterGainDial,&QDial::valueChanged,this,&MainWindow::OnFilterChangeLevel);

    //Controlli relativi all'input
    connect(ui->m_inputLevelDial,&QDial::valueChanged,this,&MainWindow::OnInputChangeLevel);
    connect(ui->m_inputActive,&QCheckBox::stateChanged,this,&MainWindow::OnInputChangeAbilitation);
    connect(ui->m_inputFiltered,&QCheckBox::stateChanged,this,&MainWindow::OnInputFilterSignal);

    //Controlli relativi al generatore sinusoidale di sinistra
    connect(ui->m_leftEnable,&QCheckBox::stateChanged,this,&MainWindow::OnLeftOscAttiv);
    connect(ui->m_leftFiltered,&QCheckBox::stateChanged,this,&MainWindow::OnLeftOscFilterSig);
    connect(ui->m_leftLevelDial,&QDial::valueChanged,this,&MainWindow::OnLeftOscLevel);
    connect(ui->m_left_FreqDial,&QDial::valueChanged,this,&MainWindow::OnLeftOscFreq);

    //Controlli relativi al generatore sinusoidale di destra
    connect(ui->m_rightEnable,&QCheckBox::stateChanged,this,&MainWindow::OnRightOscAttiv);
    connect(ui->m_rightFiltered,&QCheckBox::stateChanged,this,&MainWindow::OnRightOscFilterSig);
    connect(ui->m_rightLevelDial,&QDial::valueChanged,this,&MainWindow::OnRightOscLevel);
    connect(ui->m_rightFreqDial,&QDial::valueChanged,this,&MainWindow::OnRightOscFreq);

    //inizializzazione dell'interfaccia grafica ai valori attuati
    ui->m_filterCutoffDial->setValue(m_filterParamsLocal->CutoffFrequency);
    ui->m_filterGainDial->setValue(m_filterParamsLocal->Gain * 100);
    ui->m_FilterEnable->setChecked(m_filterParamsLocal->Active);
    ui->m_filterGainLabel->setText(QString::number(20 * log10(m_filterParamsLocal->Gain),'g',2) + " [dB]");

    ui->m_leftEnable->setChecked(m_leftOscParamsLocal->Active);
    ui->m_leftFiltered->setChecked(m_leftOscParamsLocal->ToBeFiltered);
    ui->m_leftLevelDial->setValue(m_leftOscParamsLocal->Gain * 100);
    ui->m_left_FreqDial->setValue(m_leftOscParamsLocal->Frequency);
    ui->m_leftLevelLabel->setText(QString::number(20 * log10(m_leftOscParamsLocal->Gain),'g',2) + " [dB]");

    ui->m_rightEnable->setChecked(m_rightOscParamsLocal->Active);
    ui->m_rightFiltered->setChecked(m_rightOscParamsLocal->ToBeFiltered);
    ui->m_rightLevelDial->setValue(m_rightOscParamsLocal->Gain * 100);
    ui->m_rightFreqDial->setValue(m_rightOscParamsLocal->Frequency);
    ui->m_rightLevelLabel->setText(QString::number(20 * log10(m_rightOscParamsLocal->Gain),'g',2) + " [dB]");

    ui->m_inputActive->setChecked(m_inputParamsLocal->Active);
    ui->m_inputFiltered->setChecked(m_inputParamsLocal->ToBeFiltered);
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
        ui->m_start->setText("Ferma Streaming");
        break;
    case Stopped:
    case Paused:
        ui->m_start->setText("Avvia Streaming");
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
        l_res = QString::number(l_khz) + " KHz"; // + QString.number(p_freq-(p_freq%1000))
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

void MainWindow::OnInputChangeAbilitation(int p_state)
{
    m_inputParamsLocal->Active = p_state == Qt::Checked ? true : false;
    m_streamController->UpdateInputParams(m_inputParamsLocal);
}

void MainWindow::OnInputFilterSignal(int p_state)
{
    m_inputParamsLocal->ToBeFiltered = p_state == Qt::Checked ? true : false;
    m_streamController->UpdateInputParams(m_inputParamsLocal);
}

void MainWindow::OnInputChangeLevel(double p_level)
{
    ui->m_inputLevelLabel->setText(QString::number(20 * log10(p_level/100),'g',2) + " [dB]");
    m_inputParamsLocal->Gain = p_level/100;
    m_streamController->UpdateInputParams(m_inputParamsLocal);
}


//Sinusoidale sinistra

void MainWindow::OnLeftOscLevel(double p_level)
{
    ui->m_leftLevelLabel->setText(QString::number(20 * log10(p_level/100),'g',2) + " [dB]");
    m_leftOscParamsLocal->Gain = p_level/100;
    m_streamController->UpdateLeftOscillatorsParams(m_leftOscParamsLocal);
}

void MainWindow::OnLeftOscFreq(double p_freq)
{
    ui->m_left_FreqLabel->setText(HertzToString(p_freq));
    m_leftOscParamsLocal->Frequency = p_freq;
    m_streamController->UpdateLeftOscillatorsParams(m_leftOscParamsLocal);
}

void MainWindow::OnLeftOscAttiv(int p_state)
{
    m_leftOscParamsLocal->Active = p_state == Qt::Checked ? true : false;
    m_streamController->UpdateLeftOscillatorsParams(m_leftOscParamsLocal);
}

void MainWindow::OnLeftOscFilterSig(int p_state)
{
    m_leftOscParamsLocal->ToBeFiltered = p_state == Qt::Checked ? true : false;
    m_streamController->UpdateLeftOscillatorsParams(m_leftOscParamsLocal);
}


//Sinusoidale destra

void MainWindow::OnRightOscLevel(double p_level)
{
    ui->m_rightLevelLabel->setText(QString::number(20 * log10(p_level/100),'g',2) + " [dB]");
    m_rightOscParamsLocal->Gain = p_level/100;
    m_streamController->UpdateRightOscillatorsParams(m_rightOscParamsLocal);
}

void MainWindow::OnRightOscFreq(double p_freq)
{
    ui->m_rightFreqLabel->setText(HertzToString(p_freq));
    m_rightOscParamsLocal->Frequency = p_freq;
    m_streamController->UpdateRightOscillatorsParams(m_rightOscParamsLocal);
}

void MainWindow::OnRightOscAttiv(int p_state)
{
    m_rightOscParamsLocal->Active = p_state == Qt::Checked ? true : false;
    m_streamController->UpdateRightOscillatorsParams(m_rightOscParamsLocal);
}

void MainWindow::OnRightOscFilterSig(int p_state)
{
    m_rightOscParamsLocal->ToBeFiltered = p_state == Qt::Checked ? true : false;
    m_streamController->UpdateRightOscillatorsParams(m_rightOscParamsLocal);
}
