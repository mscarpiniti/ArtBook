#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "recorder.h"
#include "player.h"
#include <QObject>
#include <string>

typedef enum {
    Stopped = 0,
    Playing,
    Recording
} State;

typedef enum {
    Open,
    Save
} openMode;

class controller : public QObject {
    Q_OBJECT
public:
    static inline controller* GetInstance() {
        if (thisInstance == nullptr)
            thisInstance = new controller();
        return thisInstance;
    }
    ~controller();
    inline State GetStatus() {return m_currentStatus; }

    void Play();
    void Stop();
    void Rec();


    QString getAudioFile() const;
    void setAudioFile(const QString &value);

private:
    controller();
    bool m_fileChosen;
    static controller* thisInstance;
    State m_currentStatus;
    Recorder* m_recorder;
    player* m_player;
    QString m_audioFile;

signals:
    void NotifyStatusChange(State);
    void UpdatePlayProgress(int);
    QString OpenFile(openMode);

public slots:
    void handlePlayProgress(long long);
    void playerFinished();
};

#endif // CONTROLLER_H
