#include "player.h"

player::player() {
    m_playerParams = new PlayerParameters();
    m_playerParams->thisInstance = this;
}

player::~player(){
    delete m_playerParams;
}

int player::paCallback(const void* p_inputBuffer, void* p_outputBuffer,
                         unsigned long p_framesPerBuffer,
                         const PaStreamCallbackTimeInfo* p_timeInfo,
                         PaStreamCallbackFlags p_statusFlags, void* p_userData)
{
    SAMPLE *l_out = (SAMPLE*)p_outputBuffer;
    (void) p_inputBuffer;
    (void) p_timeInfo;
    (void) p_statusFlags;
    PlayerParameters* l_pParams = static_cast<PlayerParameters*>(p_userData);

    sf_count_t seekResult = 0;
    sf_count_t progress = 0;

    seekResult = sf_seek(l_pParams->m_sndFile, l_pParams->m_currentPosition, SEEK_SET);
    if (seekResult < 0 || l_pParams->m_dontStopPlaying == false) {
        l_pParams->thisInstance->playEnd();
        l_pParams->m_currentPosition = 0;
        return paComplete;
    }
    sf_readf_float(l_pParams->m_sndFile,l_out,p_framesPerBuffer);

    l_pParams->m_currentPosition += p_framesPerBuffer;

    progress = l_pParams->m_sfinfo.frames - l_pParams->m_currentPosition;
    l_pParams->thisInstance->playProgress(progress);

    return paContinue;
}

void player::play(std::string fileName)
{

    m_playerParams->m_sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_PCM_24);
    m_playerParams->m_sfinfo.channels = 2;
    m_playerParams->m_sfinfo.samplerate = SAMPLE_RATE;
    m_playerParams->m_sndFile = sf_open(fileName.c_str(), SFM_READ, &m_playerParams->m_sfinfo);

    Pa_Initialize();

    m_outputParameters.device = Pa_GetDefaultOutputDevice();
    m_outputParameters.channelCount = 2;
    m_outputParameters.sampleFormat = paFloat32;
    m_outputParameters.suggestedLatency = Pa_GetDeviceInfo(m_outputParameters.device)->defaultLowOutputLatency;
    m_outputParameters.hostApiSpecificStreamInfo = nullptr;

    m_playerParams->m_dontStopPlaying = true;

    Pa_OpenStream( &m_stream,
                   nullptr,
                   &m_outputParameters,
                   SAMPLE_RATE,
                   FRAMES_PER_BUFFER,
                   paClipOff,
                   player::paCallback,
                   m_playerParams );

    Pa_StartStream(m_stream);
}

void player::stop()
{
    m_playerParams->m_dontStopPlaying = false;
    do {} while(Pa_IsStreamActive(m_stream));
    Pa_StopStream(m_stream);
    Pa_CloseStream(m_stream);
    Pa_Terminate();
    sf_close(m_playerParams->m_sndFile);
    m_playerParams->m_currentPosition = 0;
}

