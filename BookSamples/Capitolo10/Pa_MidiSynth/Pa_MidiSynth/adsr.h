#ifndef ADSR_H
#define ADSR_H

#include <QObject>

enum AdsrParams {
    Adsr_AttackTime,
    Adsr_DecayTime,
    Adsr_DecayLevel,
    Adsr_SustainTime,
    Adsr_SustainLevel,
    Adsr_ReleaseTime
};

class adsr
{
private:
    double m_atk_time;
    double m_dcy_time;
    double m_dcy_lev;
    double m_sus_time;
    double m_sus_lev;
    double m_rls_time;
    bool m_is_playing;
    bool m_note_pressed;
    double m_rls_level;
    double m_out_value;

    double m_dcy_const;
    double m_dcy_factor;
    double m_sus_const;
    double m_sus_factor;

    unsigned long int m_adsr_time;

private:
    void updateDcyFactors();
    void updateSusFactors();

public:
    adsr();
    void SetParameter(AdsrParams, double);
    void NoteReleased();
    void NotePressed();
    bool IsPlaying();
    double GetLevel(unsigned long int);
};

#endif // ADSR_H
