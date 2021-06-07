#include "recorder.h"


Recorder::Recorder()
{
    m_globalParams = new RecorderParameters();
    m_globalParams->audioBuff = nullptr;
}

void Recorder::Record(std::string filename)
{
    m_sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_PCM_24);
    m_sfinfo.channels = 2;
    m_sfinfo.samplerate = SAMPLE_RATE;

    m_sndfile = sf_open(filename.c_str(), SFM_WRITE, &m_sfinfo);
    m_globalParams->audioBuff = new std::vector<SAMPLE>(NUM_CANALI * SAMPLE_RATE);
    m_globalParams->lastElemWritten = 0;

    Pa_Initialize();

    m_inputParameters.device = Pa_GetDefaultInputDevice();
    m_inputParameters.channelCount = 2;
    m_inputParameters.sampleFormat = paFloat32;
    m_inputParameters.suggestedLatency = Pa_GetDeviceInfo(m_inputParameters.device)->defaultLowInputLatency;
    m_inputParameters.hostApiSpecificStreamInfo = nullptr;

    m_outputParameters.device = Pa_GetDefaultOutputDevice();
    m_outputParameters.channelCount = 2;
    m_outputParameters.sampleFormat = paFloat32;
    m_outputParameters.suggestedLatency = Pa_GetDeviceInfo(m_outputParameters.device)->defaultLowOutputLatency;
    m_outputParameters.hostApiSpecificStreamInfo = nullptr;

    Pa_OpenStream( &m_stream,
                   &m_inputParameters,
                   &m_outputParameters,
                   SAMPLE_RATE,
                   FRAMES_PER_BUFFER,
                   paClipOff,
                   Recorder::paCallback,
                   m_globalParams );

    Pa_StartStream(m_stream);
}

void Recorder::Stop()
{
    Pa_StopStream(m_stream);
    Pa_CloseStream(m_stream);
    Pa_Terminate();
    sf_write_float(m_sndfile,m_globalParams->audioBuff->data(),static_cast<sf_count_t>(m_globalParams->audioBuff->size()));
    sf_close(m_sndfile);
}


int Recorder::paCallback(const void* p_inputBuffer, void* p_outputBuffer,
                         unsigned long p_framesPerBuffer,
                         const PaStreamCallbackTimeInfo* p_timeInfo,
                         PaStreamCallbackFlags p_statusFlags, void* p_userData)
{
    SAMPLE *in = (SAMPLE*)p_inputBuffer;
    SAMPLE *out = (SAMPLE*)p_outputBuffer;
    (void) p_timeInfo;
    (void) p_statusFlags;

    RecorderParameters* params = static_cast<RecorderParameters*>(p_userData);
    std::vector<SAMPLE>* audioBuffer = params->audioBuff;


    if (audioBuffer->size() <= params->lastElemWritten + (2 *p_framesPerBuffer)) {
        audioBuffer->resize(audioBuffer->size() + NUM_CANALI * SAMPLE_RATE);
    }

    for (uint k=0; k < (NUM_CANALI * p_framesPerBuffer); k++) {
        *out++ = *in;
        (*params->audioBuff)[params->lastElemWritten++] = (*in++);
    }


    return paContinue;
}
