#ifndef MIXER_H
#define MIXER_H

#include <vector>
#include "voice.h"

#define NOTES 61

enum Parameters {
    Osc1_wave,
    Osc1_level,
    Osc2_wave,
    Osc2_level,
    Osc2_detune,
    NoiseLevel,
    LpfCutoff,
    LpfResonance,
    Pitch,
    ModulationUp,
    ModulationDown,
    Sustain,
    Atk_time,
    Dcy_time,
    Dcy_lev,
    Sust_time,
    Sust_lev,
    Rel_time,
    Unknown
};

using namespace std;

class VoiceBank
{
private:
    Voice* m_vbanks[NOTES] = {0};
    float m_currentCutoff;
    float m_currentPitch;
    float m_currentModulation;
    bool m_pedal;
    void RemoveDamped();
    WaveForms Osc1W;
    WaveForms Osc2W;
    float m_detune;
    float m_whiteNoiseLevel;
public:
    VoiceBank();
    ~VoiceBank();
    float NewLeftSample();
    float NewRightSample();
    void PlayVoice(int, int);
    void MuteVoice(int);
    void SetParameter(Parameters,float,int);
};

#endif // MIXER_H
