#include "adsr.h"

void adsr::updateDcyFactors() {
    m_dcy_const = ((m_atk_time * (1 - m_dcy_lev)) / m_dcy_time) + 1;
    m_dcy_factor = (m_dcy_lev - 1)/m_dcy_time;
}

void adsr::updateSusFactors() {
    m_sus_const = m_dcy_lev - (m_dcy_time * ((m_sus_lev - m_dcy_lev)/(m_sus_time - m_dcy_time)));
    m_sus_factor = (m_sus_lev - m_dcy_lev)/(m_sus_time - m_dcy_time);
}

adsr::adsr():
    m_dcy_const(0),
    m_dcy_factor(0),
    m_sus_const(0),
    m_sus_factor(0)
{
    m_note_pressed = false;
    m_is_playing = false;
    m_rls_level = 0;
    m_out_value = 0;
}

void adsr::SetParameter(AdsrParams p_param, double p_val)
{
    switch(p_param) {
    case Adsr_AttackTime:
        m_atk_time = p_val * 44.1;
        break;
    case Adsr_DecayTime:
        m_dcy_time = p_val * 44.1;
        break;
    case Adsr_DecayLevel:
        m_dcy_lev = p_val;
        break;
    case Adsr_SustainTime:
        m_sus_time = p_val * 44.1;
        break;
    case Adsr_SustainLevel:
        m_sus_lev = p_val;
        break;
    case Adsr_ReleaseTime:
        m_rls_time = p_val * 44.1;
        break;
    default:
        break;
    }

    updateDcyFactors();
    updateSusFactors();
}

void adsr::NoteReleased()
{
    m_note_pressed = false;
    m_rls_level = m_out_value;
    m_adsr_time = 0;
}

void adsr::NotePressed()
{
    m_note_pressed = true;
    m_is_playing = true;
    m_rls_level = 0;
    m_out_value = 0;

}

bool adsr::IsPlaying()
{
    return m_is_playing;
}

double adsr::GetLevel(unsigned long int p_time)
{
    if (m_note_pressed) {
        if (p_time <= m_atk_time) {
            m_out_value = ((double)p_time/m_atk_time);
        } else if (p_time > m_atk_time && p_time < (m_atk_time + m_dcy_time)) {
            m_out_value = (p_time * m_dcy_factor) + m_dcy_const;
        } else if (p_time > (m_atk_time + m_dcy_time) && p_time < (m_atk_time + m_dcy_time + m_sus_time)) {
            m_out_value = (p_time * m_sus_factor) + m_sus_const;
        }
    } else {
        m_out_value = m_rls_level - (m_adsr_time++ * (m_rls_level/m_rls_time));
    }
    if (!m_note_pressed && m_out_value <= 0) {
        m_is_playing = false;
    }
    return m_out_value;
}
