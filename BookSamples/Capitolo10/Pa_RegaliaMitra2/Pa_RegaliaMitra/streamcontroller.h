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

    SAMPLE LeftInputLine[2] = {0.0, 0.0};
    SAMPLE LeftRLine[2] = {0.0, 0.0};
    SAMPLE RightInputLine[2] = {0.0, 0.0};
    SAMPLE RightRLine[2] = {0.0, 0.0};
} RegMitraParameters;

typedef struct
{
    bool Active = true;
    bool ToBeFiltered = true;
    float Gain = 0.8;
} InputParameters;

typedef struct
{
    bool ToBeFiltered = false;
    bool Active = true;
    float Amplitude = 1.0;
    float Gain = 0.2;
    float Frequency = 220; //[Hz]
    float PhaseShift = 0.0;

    int TimelineCounter = 0;

} OscillatorParameters;

typedef enum
{
    Stopped,
    Paused,
    Playing
} StreamStatus;

typedef struct
{
    StreamStatus StreamingStatus;
    OscillatorParameters LeftOscillator;
    OscillatorParameters RightOscillator;
    RegMitraParameters Filter;
    InputParameters Input;
} GlobalParameters;

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

public:
    explicit StreamController(QObject *parent = 0);
    static StreamController* GetInstance();
    void InitController();
    void StartStreaming();
    void StopStreaming();
    void ReleaseResources();
    StreamStatus GetStreamStatus();

    void UpdateFilterParams(RegMitraParameters*);
    void UpdateInputParams(InputParameters*);
    void UpdateLeftOscillatorsParams(OscillatorParameters*);
    void UpdateRightOscillatorsParams(OscillatorParameters*);

signals:
    void NewStreamerStatus(StreamStatus);
};

#endif // STREAMCONTROLLER_H
