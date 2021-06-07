#ifndef PLAYER_H
#define PLAYER_H

#include "portaudio.h"
#include "audioio.h"
#include <string>
#include <QObject>
#include "sndfile.h"

class player;

typedef enum {
    P_Play,
    P_Stop
} playerStatus;

typedef struct {
    player* thisInstance;
    SF_INFO m_sfinfo;
    SNDFILE* m_sndFile;
    bool m_dontStopPlaying;
    sf_count_t m_currentPosition;
} PlayerParameters;

class player : public QObject
{
    Q_OBJECT
private:

    static int paCallback(const void* inputBuffer, void* outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags, void* userData);

    PaStreamParameters m_outputParameters;
    PaStreamParameters m_inputParameters;
    PaStream* m_stream;
    PlayerParameters* m_playerParams;

public:
    player();
    ~player();
    void play(std::string fileName);
    void stop();
    //void localPlayEnd();
    //void localPlayProgress(long long);
signals:
    void playEnd();
    void playProgress(long long);
};

#endif // PLAYER_H
