#include "streamcontroller.h"
#include <cstring>

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
    RegMitraParameters* l_Filter = &l_globalP->Filter;
    InputParameters* l_Input = &l_globalP->Input;

    for (unsigned int k = 0; k < p_framesPerBuffer; k++)
    {
        std::memset(&g_sv, 0.0, sizeof(StreamVariables));

        if (l_Input->Active)
        {
            if (l_Input->ToBeFiltered && l_Filter->Active)
            {
                g_sv.l_leftFilterInput  += l_Input->Gain * (*in++);
                g_sv.l_rightFilterInput += l_Input->Gain * (*in++);
            }
            else
            {
                g_sv.l_leftResult  += l_Input->Gain * (*in++);
                g_sv.l_rightResult += l_Input->Gain * (*in++);
            }
        }

        if (l_Filter->Active && g_sv.l_leftFilterInput != 0.0 && g_sv.l_rightFilterInput != 0.0)
        {

            //Equalizzatore di Regalia-Mitra del 1° ordine
            float tanK = tan((M_PI * l_Filter->CutoffFrequency)/g_sampleRate);
            g_sv.l_k = (tanK - 1)/(tanK + 1);
            g_sv.l_LeftRn  = (-1 * g_sv.l_k * g_sv.l_leftFilterInput) - l_Filter->LeftInputLine - (g_sv.l_k * l_Filter->LeftRLine);
            g_sv.l_RightRn = (-1 * g_sv.l_k *g_sv.l_rightFilterInput) - l_Filter->RightInputLine - (g_sv.l_k * l_Filter->RightRLine);

            //Shift manuale dei valori sulle linee di ritardo
            l_Filter->LeftInputLine = g_sv.l_leftFilterInput;
            l_Filter->LeftRLine = g_sv.l_LeftRn;
            l_Filter->RightInputLine = g_sv.l_rightFilterInput;
            l_Filter->RightRLine = g_sv.l_RightRn;

            //Parte comune alle due versioni
            g_sv.l_leftFilterOut  = ((0.5 + 0.5 * l_Filter->Gain) * g_sv.l_leftFilterInput) + ((0.5 - 0.5 * l_Filter->Gain) * g_sv.l_LeftRn);
            g_sv.l_rightFilterOut = ((0.5 + 0.5 * l_Filter->Gain) * g_sv.l_rightFilterInput) + ((0.5 - 0.5 * l_Filter->Gain) * g_sv.l_RightRn);

        }
        else
        {
            g_sv.l_leftFilterOut  = 0.0;
            g_sv.l_rightFilterOut = 0.0;
        }

        *out++ = g_sv.l_leftResult  + g_sv.l_leftFilterOut;
        *out++ = g_sv.l_rightResult + g_sv.l_rightFilterOut;
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
