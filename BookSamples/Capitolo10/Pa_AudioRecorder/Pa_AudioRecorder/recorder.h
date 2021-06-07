#ifndef RECORDER_H
#define RECORDER_H

#include "portaudio.h"
#include "audioio.h"
#include <QString>
#include "sndfile.h"
#include <vector>

typedef enum {
    R_Recor,
    R_Stop
} RecorderStatus;

typedef struct {
    std::vector<SAMPLE>* audioBuff;
    uint64_t lastElemWritten;
} RecorderParameters;

class Recorder
{
private:
    static int paCallback(const void* inputBuffer, void* outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags, void* userData);

    PaStreamParameters m_outputParameters;
    PaStreamParameters m_inputParameters;
    PaStream* m_stream;
    RecorderParameters* m_globalParams;

    SF_INFO m_sfinfo;
    SNDFILE* m_sndfile;

public:
    Recorder();
    void Record(std::string);
    void Stop();
};

#endif // RECORDER_H
