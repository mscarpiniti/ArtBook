#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <string>
#include <QString>

MainWindow::MainWindow(QWidget *p_parent)
    : QMainWindow(p_parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
}

void MainWindow::SetMidiSynthReference(midi_synth * p_ref)
{
    if (p_ref != nullptr) {
        m_theSynth = p_ref;
    } else {
        exit(-1);
    }
}

void MainWindow::InitAll()
{
    //init midi engine
    m_theSynth->Init();
    std::vector<std::string> lista = m_theSynth->GetMidiDeviceList();
    for (size_t k = 0; k < lista.size(); k++) {
        m_ui->midiCombo->addItem(QString::fromUtf8(lista[k].c_str()));
    }
    m_ui->midiCombo->setCurrentText(QString::fromUtf8(m_theSynth->GetCurrentMidiDevice().c_str()));
    QObject::connect(m_ui->midiCombo, &QComboBox::currentTextChanged, this, &MainWindow::SetMidiInput);

    m_ui->osc1_wave->addItem("Sine");
    m_ui->osc2_wave->addItem("Sine");
    m_ui->osc1_wave->addItem("SawTooth");
    m_ui->osc2_wave->addItem("SawTooth");
    m_ui->osc1_wave->addItem("Square");
    m_ui->osc2_wave->addItem("Square");

    QObject::connect(m_ui->osc1_wave, &QComboBox::currentTextChanged, m_theSynth, &midi_synth::Osc1_Wave);
    QObject::connect(m_ui->osc2_wave, &QComboBox::currentTextChanged, m_theSynth, &midi_synth::Osc2_Wave);
    QObject::connect(m_ui->level_OscA, &QDial::valueChanged, m_theSynth, &midi_synth::Osc1_Level);
    QObject::connect(m_ui->level_OscB, &QDial::valueChanged, m_theSynth, &midi_synth::Osc2_Level);
    QObject::connect(m_ui->level_Cutoff, &QDial::valueChanged, m_theSynth, &midi_synth::Cutoff);
    QObject::connect(m_ui->level_Detune, &QDial::valueChanged, m_theSynth, &midi_synth::Osc2_Detune);
    QObject::connect(m_ui->level_Cutoff, &QDial::valueChanged, this, &MainWindow::SetFreq);
    QObject::connect(m_ui->level_Noise, &QDial::valueChanged, m_theSynth, &midi_synth::Noise_Level);

    QObject::connect(m_ui->atk_time, &QDial::valueChanged, m_theSynth, &midi_synth::Attack_time);
    QObject::connect(m_ui->dcy_time, &QDial::valueChanged, m_theSynth, &midi_synth::Decay_time);
    QObject::connect(m_ui->dcy_lev, &QDial::valueChanged, m_theSynth, &midi_synth::Decay_level);
    QObject::connect(m_ui->sus_dur, &QDial::valueChanged, m_theSynth, &midi_synth::Sus_time);
    QObject::connect(m_ui->sus_lev, &QDial::valueChanged, m_theSynth, &midi_synth::Sus_level);
    QObject::connect(m_ui->rls_time, &QDial::valueChanged, m_theSynth, &midi_synth::Release_Time);

    //Propagate the current IU (default) levels in the synth instance
    m_theSynth->Cutoff(m_ui->level_Cutoff->value());
    m_theSynth->Osc2_Detune(m_ui->level_Detune->value());
    m_theSynth->Noise_Level(m_ui->level_Noise->value());
    m_theSynth->Attack_time(m_ui->atk_time->value());
    m_theSynth->Decay_time(m_ui->dcy_time->value());
    m_theSynth->Decay_level(m_ui->dcy_lev->value());
    m_theSynth->Sus_time(m_ui->sus_dur->value());
    m_theSynth->Sus_level(m_ui->sus_lev->value());
    m_theSynth->Release_Time(m_ui->rls_time->value());
}


MainWindow::~MainWindow()
{
    delete m_theSynth;
    delete m_ui;
}

void MainWindow::SetMidiInput(const QString &p_text)
{
    m_theSynth->SetMidiDevice(std::string(p_text.toUtf8()));

}

void MainWindow::SetFreq(float val)
{
    m_ui->label_value_Cutoff->setText(QString::number(val) + "[Hz]");
}


