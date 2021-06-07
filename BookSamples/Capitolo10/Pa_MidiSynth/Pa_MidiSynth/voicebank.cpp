
#include "voicebank.h"

void VoiceBank::RemoveDamped()
{
    for (int nt = 0; nt < NOTES; nt++) {
        if (m_vbanks[nt]->IsActive()) {
            if (m_vbanks[nt]->GetDamped()) {
                delete m_vbanks[nt];
                m_vbanks[nt] = 0;
            }
        }

    }
}

VoiceBank::VoiceBank()
{
    m_pedal = false;
    Osc1W = Sine;
    Osc2W = Sine;
    m_detune = 0.0;
    m_whiteNoiseLevel = 0.0;
    for (int nt = 0; nt < NOTES; nt++) {
            m_vbanks[nt] = new Voice(nt + 36, 0, Osc1W, Osc2W);
    }
}

VoiceBank::~VoiceBank()
{
    for (int nt = 0; nt < NOTES; nt++) {
        if (m_vbanks[nt] != nullptr) {
            delete m_vbanks[nt];
        }
    }

}

float VoiceBank::NewLeftSample()
{
    float sample = 0.0;
    for (int nt = 0; nt < NOTES; nt++) {
        if (m_vbanks[nt]->IsActive()) {
            sample += m_vbanks[nt]->GetNewSamples()[0];
        }
    }
    return sample / 10;
}

float VoiceBank::NewRightSample()
{
    float sample = 0.0;
    for (int nt = 0; nt < NOTES; nt++) {
        if (m_vbanks[nt]->IsActive()) {
            sample += m_vbanks[nt]->GetNewSamples()[1];
        }
    }
    return sample / 10;
}


void VoiceBank::PlayVoice(int p_note,int p_velocity)
{
    if (p_note >= 36 && p_note <= 96) {
        m_vbanks[p_note - 36]->PlayNote(Osc1W,
                                      Osc2W,
                                      p_velocity,
                                      m_detune);
    }
}

void VoiceBank::MuteVoice(int p_note)
{
    if (p_note >= 36 && p_note <= 96) {
        if (m_pedal == false) {
            m_vbanks[p_note - 36]->MuteNote();
        } else {
            m_vbanks[p_note - 36]->SetDamped();
        }
    }
}

void VoiceBank::SetParameter(Parameters p_param, float p_value, int p_gen)
{
    int nt = 0;

    switch (p_param) {
    case Osc1_wave:
        Osc1W = (WaveForms)p_gen;
        for (nt = 0; nt < NOTES; nt++) {
                m_vbanks[nt]->Osc1->SetWave((WaveForms)p_gen);
        }
        break;
    case Osc1_level:
        for (nt = 0; nt < NOTES; nt++) {
                m_vbanks[nt]->Osc1->SetLevel(p_value);
        }
        break;
    case Osc2_wave:
        Osc2W = (WaveForms)p_gen;
        for (nt = 0; nt < NOTES; nt++) {
                m_vbanks[nt]->Osc2->SetWave((WaveForms)p_gen);
        }
        break;
    case Osc2_level:
        for (nt = 0; nt < NOTES; nt++) {
                m_vbanks[nt]->Osc2->SetLevel(p_value);
        }
        break;
    case Osc2_detune:
        m_detune = p_value;
        for (nt = 0; nt < NOTES; nt++) {
                m_vbanks[nt]->SetDetune(p_value);
        }
        break;
    case NoiseLevel:
        m_whiteNoiseLevel = p_value;
        for (nt = 0; nt < NOTES; nt++) {
                m_vbanks[nt]->SetNoiseLevel(p_value);
        }
        break;
    case LpfCutoff:
        m_currentCutoff = p_value;
        for (nt = 0; nt < NOTES; nt++) {
                m_vbanks[nt]->LPF->ChangeFreq(p_value);
        }
        break;
    case LpfResonance:

        break;
    case ModulationUp:
        for (nt = 0; nt < NOTES; nt++) {
            m_vbanks[nt]->SetModulation(p_gen/127.0);
        }
        break;
    case ModulationDown:
        for (nt = 0; nt < NOTES; nt++) {
            m_vbanks[nt]->SetModulation(p_gen/127.0);
        }
        break;
    case Pitch:
        p_value = (int)(p_value * 100);
        p_value = p_value/100;
        m_currentPitch = p_value;
        for (nt = 0; nt < NOTES; nt++) {
            m_vbanks[nt]->SetPitch(m_currentPitch);
        }
        break;
    case Sustain:
        if (p_gen > 64) {
            m_pedal = true;
        } else {
            m_pedal = false;
            RemoveDamped();
        }
        break;
    case Atk_time:
        for (nt = 0; nt < NOTES; nt++) {
                m_vbanks[nt]->ADSR->SetParameter(Adsr_AttackTime,p_value);
        }
        break;
    case Dcy_time:
        for (nt = 0; nt < NOTES; nt++) {
                m_vbanks[nt]->ADSR->SetParameter(Adsr_DecayTime,p_value);
        }
        break;
    case Dcy_lev:
        for (nt = 0; nt < NOTES; nt++) {
                m_vbanks[nt]->ADSR->SetParameter(Adsr_DecayLevel,p_value);
        }
        break;
    case Sust_time:
        for (nt = 0; nt < NOTES; nt++) {
                m_vbanks[nt]->ADSR->SetParameter(Adsr_ReleaseTime,p_value);
        }
        break;
    case Sust_lev:
        for (nt = 0; nt < NOTES; nt++) {
                m_vbanks[nt]->ADSR->SetParameter(Adsr_SustainLevel,p_value);
        }
        break;
    case Rel_time:
        for (nt = 0; nt < NOTES; nt++) {
                m_vbanks[nt]->ADSR->SetParameter(Adsr_ReleaseTime,p_value);
        }
        break;
    case Unknown:

        break;
    }
}



