#ifndef MIDI_SYNTH_H
#define MIDI_SYNTH_H

#include <QObject>
#include <QString>
#include <portaudio.h>
#include "midi_event_handler.h"
#include "voicebank.h"
#include <vector>
#include <mutex>

struct synth_parameters {
    PmQueue* midiEvents;
    VoiceBank* voices;
};

class midi_synth : public QObject
{
    Q_OBJECT

private:
    midi_events_handler* m_midi_handler;
    PaStream *m_aStream;
    PaStreamParameters m_outputParameters;
    synth_parameters* m_params;


public:
    explicit midi_synth(QObject *parent = nullptr);
    ~midi_synth();
    void Init();

public slots:
    std::vector<std::string> &GetMidiDeviceList();
    void SetMidiDevice(std::string);
    std::string GetCurrentMidiDevice();

    // Low pass filter
    void Cutoff(double);

    // Oscillator 1
    void Osc1_Wave(QString);
    void Osc1_Level(double);

    //Oscillator 2
    void Osc2_Wave(QString);
    void Osc2_Level(double);
    void Osc2_Detune(double);

    //White noise
    void Noise_Level(double);

    //ADSR
    void Attack_time(double);
    void Decay_time(double);
    void Decay_level(double);
    void Sus_time(double);
    void Sus_level(double);
    void Release_Time(double);

};

#endif // MIDI_SYNTH_H
