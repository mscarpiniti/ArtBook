#ifndef VOICE_H
#define VOICE_H

#include "math.h"
#include <cstdlib>
#include "adsr.h"

#define PITCH_OFFSET 2 //in semitones

enum WaveForms {
    Sine,
    SawTooth,
    Square
};

/********************************************************************
    CLASS OSCILLATOR
********************************************************************/

class Oscillator {
private:
    WaveForms m_wave;
    float m_baseFreq;
    float m_currentFreq;
    float m_detune;
    float m_pitch;
    float m_level;
    float m_lastSample;
    unsigned long int m_lastTm;

    float m_Tcos, m_Tsin;
    float m_theta_increment;
    float m_beta;
    float m_alpha;
    float m_Ncos, m_Nsin;

    void CalcIncParams();


public:
    Oscillator(WaveForms, float);
    void SetWave(WaveForms);
    void SetLevel(float);
    void SetDetune(float);
    void SetPitch(float);
    float GetNewSample(unsigned long int);

};

/********************************************************************
    CLASS LOWPASS
********************************************************************/

class LowPass {
private:
    float m_bandpass;
    float m_rLine[3] = {0};
    float k, Q, f0, fb, den;

    float IIR(float x, float *a, float *b, float *buffer, int N);

public:
    LowPass();
    float Process(float);
    void ChangeFreq(float);
};

/********************************************************************
    CLASS VOICE
********************************************************************/


class Voice
{
private:
    float c_pitchConst;
    int m_midNote;
    int m_velocity;
    float m_noteFreq;
    bool m_damped;
    volatile float m_currentSample[2] = {0};
    volatile float m_detune;
    unsigned long int m_time;
    float m_wnLevel;
    bool m_active;
    float m_lfoAmp;
    float m_pitchLev;
    float m_prvVal;
    float m_freqVariation;

public:
    //this choice breaks the information hiding principle
    //but declaring these properties as public makes the
    //implementation faster and our main topic is not the
    //good programming principles.

    Oscillator* Osc1;
    Oscillator* Osc2;
    LowPass* LPF;
    adsr* ADSR;

public:
    Voice(int,int,WaveForms,WaveForms);
    ~Voice();
    void PlayNote(WaveForms,WaveForms,int,float);
    void MuteNote();
    int GetNote();
    bool IsActive() {return ADSR->IsPlaying();}
    void SetDamped() {m_damped = true;}
    bool GetDamped() {return m_damped;}
    volatile float* GetNewSamples();
    void SetDetune(float);
    void SetNoiseLevel(float);
    void SetAdsrParam(AdsrParams, double val);
    void SetModulation(float lev);
    void SetPitch(float val);
};

#endif // VOICE_H
