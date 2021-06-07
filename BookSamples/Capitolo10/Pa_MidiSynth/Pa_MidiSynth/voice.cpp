#include "voice.h"

/********************************************************************
    CLASS VOICE
********************************************************************/

Voice::Voice(int p_note, int p_velocity, WaveForms p_wv1, WaveForms p_wv2)
{
    m_midNote = p_note;
    m_noteFreq = (440 * pow(2,((double)(m_midNote-69)/12.0)))/4;
    m_freqVariation = 0.0;
    Osc1 = new Oscillator(p_wv1,m_noteFreq);
    Osc2 = new Oscillator(p_wv2,m_noteFreq);
    LPF = new LowPass();
    ADSR = new adsr();
    m_velocity = p_velocity;
    m_damped = false;
    m_detune = 0.0;
    m_time = 0;
    m_active = 0.0;
    m_lfoAmp = 0.0;
    m_prvVal = 0.0;
    c_pitchConst = PITCH_OFFSET * 0.059463094;
}

Voice::~Voice()
{
    delete Osc1;
    delete Osc2;
    delete LPF;
    delete ADSR;

}

void Voice::PlayNote(WaveForms p_osc1, WaveForms p_osc2, int p_velocity , float p_detune)
{
    SetDetune(p_detune);
    ADSR->NotePressed();
    m_velocity = p_velocity;
    Osc1->SetWave(p_osc1);
    Osc2->SetWave(p_osc2);
    m_time = 0;
}

void Voice::MuteNote()
{
    ADSR->NoteReleased();
}

int Voice::GetNote() {
    return m_midNote;
}

volatile float* Voice::GetNewSamples() {
    float val = (1 - (m_lfoAmp * sin(2*M_PI*(float)(m_time) * 0.00006)));
    m_currentSample[0] = val * ADSR->GetLevel(m_time) * ((float)m_velocity/127) * LPF->Process(Osc1->GetNewSample(m_time) + Osc2->GetNewSample(m_time) + m_wnLevel * (((double)rand()/RAND_MAX) - 0.5));
    m_currentSample[1] = val * ADSR->GetLevel(m_time) * ((float)m_velocity/127) * LPF->Process(Osc1->GetNewSample(m_time) + Osc2->GetNewSample(m_time) + m_wnLevel * (((double)rand()/RAND_MAX) - 0.5));
    m_time++;
    return m_currentSample;
}

void Voice::SetDetune(float p_val) {
    m_detune = (p_val - 50)/10;
    Osc2->SetDetune(m_detune);
}

void Voice::SetNoiseLevel(float val)
{
    m_wnLevel = val/70;
}

void Voice::SetAdsrParam(AdsrParams p_par, double p_val)
{
    ADSR->SetParameter(p_par,p_val);
}

void Voice::SetModulation(float p_lev)
{
    m_lfoAmp = p_lev;
}

void Voice::SetPitch(float p_val)
{
    m_freqVariation = m_noteFreq * c_pitchConst * p_val;
    Osc1->SetPitch(m_freqVariation);
    Osc2->SetPitch(m_freqVariation);
}

/********************************************************************
    CLASS OSCILLATOR
********************************************************************/

void Oscillator::CalcIncParams()
{
    m_currentFreq = m_baseFreq + m_detune + m_pitch;
    m_theta_increment = 2*M_PI * ((float)(m_currentFreq)/(float)44100);
    m_beta = sin(m_theta_increment);
    m_alpha = sin(m_theta_increment/2);
    m_alpha = 2 * m_alpha * m_alpha;
}

Oscillator::Oscillator(WaveForms p_wv, float p_baseFreq) {
    m_wave = p_wv;
    m_level = 0.5;
    m_baseFreq = p_baseFreq;
    m_currentFreq = p_baseFreq;
    m_lastTm = 0;
    m_lastSample = 0;
    m_Tcos = 1;
    m_Tsin = 0;
    CalcIncParams();
}

void Oscillator::SetWave(WaveForms p_wv)
{
    m_wave = p_wv;
}

void Oscillator::SetLevel(float p_lv)
{
    m_level = p_lv/50;
}

void Oscillator::SetDetune(float p_detune)
{
    m_detune = p_detune;
    CalcIncParams();
}

void Oscillator::SetPitch(float p_pitch)
{
    m_pitch = p_pitch;
    CalcIncParams();
}

float Oscillator::GetNewSample(unsigned long int p_tm)
{
    switch(m_wave) {
    case Sine:     
        m_Ncos = (m_alpha * m_Tcos) + (m_beta * m_Tsin);
        m_Nsin = (m_alpha * m_Tsin) - (m_beta * m_Tcos);

        m_Tcos -= m_Ncos;
        m_Tsin -= m_Nsin;

        m_lastSample -= m_Tsin;
        m_lastSample = m_lastSample/2;
        break;
    case SawTooth:
        if (m_lastSample >= 1.00) {
            m_lastSample = -1.00 ;
        }
        m_lastSample = m_lastSample + (m_currentFreq/(float)44100);
        break;
    case Square:
        if (p_tm - m_lastTm >= (float)44100/(2*m_currentFreq)) {
            m_lastTm = p_tm;
            if (m_lastSample != 1 && m_lastSample != -1) {
                m_lastSample = 1;
            } else if (m_lastSample == -1) {
                m_lastSample = 1;
            } else if (m_lastSample == 1) {
                m_lastSample = -1;
            }
        }
        break;
    }

    return m_level* m_lastSample;
}

/********************************************************************
    CLASS LOWPASS
********************************************************************/

LowPass::LowPass() {

}

float LowPass::IIR(float x, float *a, float *b, float *buffer, int N)
{
    int i;
    float y;

    buffer[0] = x;        /* Lettura dell'ingresso */

    for (i = 1; i <= N; i++)
        buffer[0] -= a[i] * buffer[i];  /* Accumulo delle uscite passate */

    y = b[N] * buffer[N];

    for (i = N - 1; i >= 0; i--)
    {
        buffer[i + 1] = buffer[i];   /* Scorrimento del buffer */
        y += b[i] * buffer[i];       /* Accumulo degli ingressi passati */
    }

    return y;
}

float LowPass::Process(float p_in)
{
    float f1 = 1;
    float f2 = m_bandpass;
    float a[3]; float b[3];

    f0 = (f1 + f2) / 2;
    fb = f2 - f1;

    Q = 1 / sqrt(2);
    den = k*k*Q + k + Q;
    a[0] = 1.0;
    a[1] = 2 * Q*(k*k - 1) / den;
    a[2] = (k*k*Q - k + Q) / den;
    b[0] = k*k*Q / den;
    b[1] = 2*k*k*Q / den;
    b[2] = k*k*Q / den;

    return IIR(p_in, a, b, m_rLine, 2);
}

void LowPass::ChangeFreq(float bandPass)
{
    k = tan(M_PI * bandPass / (float)44100);
}


