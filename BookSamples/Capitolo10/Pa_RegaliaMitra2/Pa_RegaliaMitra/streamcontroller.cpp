#include "streamcontroller.h"

StreamController::StreamController(QObject *parent) : QObject(parent)
{
    m_globalParams = new GlobalParameters();
}

StreamController* StreamController::m_thisInstance = NULL;

int StreamController::paCallback(const void* p_inputBuffer, void* p_outputBuffer,
                                 unsigned long p_framesPerBuffer,
                                 const PaStreamCallbackTimeInfo* p_timeInfo,
                                 PaStreamCallbackFlags p_statusFlags, void* p_userData)
{
    SAMPLE *out = (SAMPLE*)p_outputBuffer;
    SAMPLE *in  = (SAMPLE*)p_inputBuffer;
    (void) p_timeInfo;
    (void) p_statusFlags;

    GlobalParameters* l_globalP = (GlobalParameters*)p_userData;

    OscillatorParameters* l_LeftOscillator = &l_globalP->LeftOscillator;
    OscillatorParameters* l_RightOscillator = &l_globalP->RightOscillator;
    RegMitraParameters* l_Filter = &l_globalP->Filter;
    InputParameters* l_Input = &l_globalP->Input;

    SAMPLE l_leftResult = 0.0;
    SAMPLE l_leftFilterInput = 0.0;
    SAMPLE l_leftFilterOut = 0.0;
    SAMPLE l_LeftRn = 0.0;
    SAMPLE l_rightResult = 0.0;
    SAMPLE l_rightFilterInput = 0.0;
    SAMPLE l_rightFilterOut = 0.0;
    SAMPLE l_RightRn = 0.0;

    float  l_k = 0.0;
    float  l_sampleRate = SAMPLE_RATE;

    for (unsigned int k = 0; k < p_framesPerBuffer; k++)
    {
        l_leftResult = 0.0;
        l_rightResult = 0.0;
        l_leftFilterInput = 0.0;
        l_rightFilterInput = 0.0;

        if (l_Input->Active)
        {
            if (l_Input->ToBeFiltered && l_Filter->Active)
            {
                l_leftFilterInput  += l_Input->Gain * (*in++);
                l_rightFilterInput += l_Input->Gain * (*in++);
            }
            else
            {
                l_leftResult  += l_Input->Gain * (*in++);
                l_rightResult += l_Input->Gain * (*in++);
            }
        }

        if (l_LeftOscillator->Active)
        {
            //process leftChannel SineWave
            if(l_LeftOscillator->ToBeFiltered && l_Filter->Active)
            {
                l_leftFilterInput += l_LeftOscillator->Gain*l_LeftOscillator->Amplitude*sin(2*M_PI*(++(l_LeftOscillator->TimelineCounter))*(l_LeftOscillator->Frequency/SAMPLE_RATE));
            }
            else
            {
                l_leftResult += l_LeftOscillator->Gain*l_LeftOscillator->Amplitude*sin(2*M_PI*(++(l_LeftOscillator->TimelineCounter))*(l_LeftOscillator->Frequency/SAMPLE_RATE));
            }
        }

        if (l_RightOscillator->Active)
        {
            //process rightChennal SineWave
            if(l_RightOscillator->ToBeFiltered && l_Filter->Active)
            {
                l_rightFilterInput += l_RightOscillator->Gain*l_RightOscillator->Amplitude*sin(2*M_PI*(++(l_RightOscillator->TimelineCounter))*(l_RightOscillator->Frequency/SAMPLE_RATE));
            }
            else
            {
                l_rightResult += l_RightOscillator->Gain*l_RightOscillator->Amplitude*sin(2*M_PI*(++(l_RightOscillator->TimelineCounter))*(l_RightOscillator->Frequency/SAMPLE_RATE));
            }

        }

        if (l_Filter->Active && l_leftFilterInput != 0.0 && l_rightFilterInput != 0.0)
        {

            //Equalizzatore di Regalia-Mitra del 1° ordine
            float tanK = tan((M_PI * l_Filter->CutoffFrequency)/l_sampleRate);
            l_k = (tanK - 1)/(tanK + 1);
            l_LeftRn = (l_k * l_leftFilterInput) + l_Filter->LeftInputLine[0] + (l_k * l_Filter->LeftRLine[0]);
            l_RightRn = (l_k *l_rightFilterInput) + l_Filter->RightInputLine[0] + (l_k * l_Filter->RightRLine[0]);

            //Shift manuale dei valori sulle linee di ritardo
            l_Filter->LeftInputLine[0] = l_leftFilterInput;
            l_Filter->LeftRLine[0] = l_LeftRn;
            l_Filter->RightInputLine[0] = l_rightFilterInput;
            l_Filter->RightRLine[0] = l_RightRn;

            //Parte comune alle due versioni
            l_leftFilterOut  = ((0.5 + 0.5 * l_Filter->Gain) * l_leftFilterInput) + ((0.5 - 0.5 * l_Filter->Gain) * l_LeftRn);
            l_rightFilterOut = ((0.5 + 0.5 * l_Filter->Gain) * l_rightFilterInput) + ((0.5 - 0.5 * l_Filter->Gain) * l_RightRn);

        }
        else
        {
            l_leftFilterOut  = 0.0;
            l_rightFilterOut = 0.0;
        }

        *out++ = l_leftResult  + l_leftFilterOut;
        *out++ = l_rightResult + l_rightFilterOut;
    }

    return paContinue;
}

void StreamController::InitController()
{
    Pa_Initialize();

    m_outputParameters.device = Pa_GetDefaultOutputDevice();
    m_outputParameters.channelCount = 2;
    m_outputParameters.sampleFormat = paFloat32;
    m_outputParameters.suggestedLatency = Pa_GetDeviceInfo(m_outputParameters.device)->defaultLowOutputLatency;
    m_outputParameters.hostApiSpecificStreamInfo = NULL;

    m_inputParameters.device = Pa_GetDefaultInputDevice();
    m_inputParameters.channelCount = 2;
    m_inputParameters.sampleFormat = paFloat32;
    m_inputParameters.suggestedLatency = Pa_GetDeviceInfo(m_inputParameters.device)->defaultLowInputLatency;
    m_inputParameters.hostApiSpecificStreamInfo = NULL;

    Pa_OpenStream( &m_stream,
                   &m_inputParameters,
                   &m_outputParameters,
                   SAMPLE_RATE,
                   FRAMES_PER_BUFFER,
                   paClipOff,
                   StreamController::paCallback,
                   m_globalParams );
}

StreamController* StreamController::GetInstance()
{
    if (m_thisInstance == NULL)
    {
        m_thisInstance = new StreamController();
    }
    return m_thisInstance;
}

void StreamController::StartStreaming()
{
    if (0 == Pa_IsStreamActive(m_stream))
    {
        Pa_StartStream(m_stream);
        m_globalParams->StreamingStatus = Playing;
        emit NewStreamerStatus(Playing);
    }
}

void StreamController::StopStreaming()
{
    if (1 == Pa_IsStreamActive(m_stream))
    {
        Pa_StopStream(m_stream);
        m_globalParams->StreamingStatus = Paused;
        emit NewStreamerStatus(Paused);
    }
}

void StreamController::ReleaseResources()
{
    StopStreaming();
    Pa_CloseStream( m_stream );
    m_globalParams->StreamingStatus = Stopped;
    emit NewStreamerStatus(Stopped);
    Pa_Terminate();
}

StreamStatus StreamController::GetStreamStatus()
{
    return m_globalParams->StreamingStatus;
}

void StreamController::UpdateFilterParams(RegMitraParameters* p_params)
{
    m_globalParams->Filter.Active = p_params->Active;
    m_globalParams->Filter.CutoffFrequency = p_params->CutoffFrequency;
    m_globalParams->Filter.Gain = p_params->Gain;
}

void StreamController::UpdateInputParams(InputParameters *p_params)
{
    m_globalParams->Input.Active = p_params->Active;
    m_globalParams->Input.Gain = p_params->Gain;
    m_globalParams->Input.ToBeFiltered = p_params->ToBeFiltered;
}

void StreamController::UpdateLeftOscillatorsParams(OscillatorParameters *p_params)
{
    m_globalParams->LeftOscillator.Active = p_params->Active;
    m_globalParams->LeftOscillator.Amplitude = p_params->Amplitude;
    m_globalParams->LeftOscillator.Frequency = p_params->Frequency;
    m_globalParams->LeftOscillator.Gain = p_params->Gain;
    m_globalParams->LeftOscillator.PhaseShift = p_params->PhaseShift;
    m_globalParams->LeftOscillator.ToBeFiltered = p_params->ToBeFiltered;
}

void StreamController::UpdateRightOscillatorsParams(OscillatorParameters *p_params)
{
    m_globalParams->RightOscillator.Active = p_params->Active;
    m_globalParams->RightOscillator.Amplitude = p_params->Amplitude;
    m_globalParams->RightOscillator.Frequency = p_params->Frequency;
    m_globalParams->RightOscillator.Gain = p_params->Gain;
    m_globalParams->RightOscillator.PhaseShift = p_params->PhaseShift;
    m_globalParams->RightOscillator.ToBeFiltered = p_params->ToBeFiltered;
}
