#ifndef STREAMCONTROLLER_H
#define STREAMCONTROLLER_H

#include <QObject>
#include "audioio.h"
#include <math.h>

typedef struct
{
    bool Active = true;
    float CutoffFrequency = 400; //[Hz]
    float Gain = 1.0;

    //delay line per il filtro del 1° ordine
    SAMPLE LeftInputLine = 0.0;
    SAMPLE LeftRLine = 0.0;
    SAMPLE RightInputLine = 0.0;
    SAMPLE RightRLine = 0.0;
} RegMitraParameters;

typedef struct
{
    bool Active = true;
    bool ToBeFiltered = true;
    float Gain = 0.8;
} InputParameters;

typedef struct
{
    SAMPLE l_leftResult = 0.0;
    SAMPLE l_leftFilterInput = 0.0;
    SAMPLE l_leftFilterOut = 0.0;
    SAMPLE l_LeftRn = 0.0;
    SAMPLE l_rightResult = 0.0;
    SAMPLE l_rightFilterInput = 0.0;
    SAMPLE l_rightFilterOut = 0.0;
    SAMPLE l_RightRn = 0.0;
    SAMPLE l_k = 0.0;

} StreamVariables;

typedef enum
{
    Stopped,
    Paused,
    Playing
} StreamStatus;

typedef struct
{
    StreamStatus StreamingStatus;
    RegMitraParameters Filter;
    InputParameters Input;
} GlobalParameters;

static StreamVariables g_sv;
static float g_sampleRate = SAMPLE_RATE;

class StreamController : public QObject
{
    Q_OBJECT
private:
    static int paCallback(const void* inputBuffer, void* outputBuffer,
                    unsigned long framesPerBuffer,
                    const PaStreamCallbackTimeInfo* timeInfo,
                    PaStreamCallbackFlags statusFlags, void* userData);

    static StreamController* m_thisInstance;
    PaStreamParameters m_outputParameters;
    PaStreamParameters m_inputParameters;
    PaStream *m_stream;
    GlobalParameters* m_globalParams;
    explicit StreamController(QObject *parent = 0);
public:
    static StreamController* GetInstance();
    void InitController();
    void StartStreaming();
    void StopStreaming();
    void ReleaseResources();
    StreamStatus GetStreamStatus();
    void UpdateFilterParams(RegMitraParameters*);
    void UpdateInputParams(InputParameters*);
signals:
    void NewStreamerStatus(StreamStatus);
};

#endif // STREAMCONTROLLER_H
