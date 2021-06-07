#include "controller.h"


controller* controller::thisInstance = nullptr;

controller::~controller()
{
    delete m_player;
    delete m_recorder;
}

void controller::Play()
{
    if (m_currentStatus == Stopped && m_fileChosen == false) {
        m_audioFile = OpenFile(Open);
    }
    if (!m_audioFile.isEmpty()) {
        m_currentStatus = Playing;
        m_player->play(m_audioFile.toStdString());
        emit NotifyStatusChange(m_currentStatus);  
    }
}

void controller::Stop()
{
    if (m_currentStatus == Recording) m_recorder->Stop();
    if (m_currentStatus == Playing) m_player->stop();
    m_currentStatus = Stopped;
    emit NotifyStatusChange(m_currentStatus);
}

void controller::Rec()
{

    if (m_currentStatus == Stopped && false == m_fileChosen) {
        //chiedi per il file da aprire
        m_audioFile = OpenFile(Save);
    }
    if (!m_audioFile.isEmpty()) {       
        m_currentStatus = Recording;
        emit NotifyStatusChange(m_currentStatus);
        m_recorder->Record(m_audioFile.toStdString());
    }
}

controller::controller()
{
    m_currentStatus = Stopped;
    m_fileChosen = false;
    m_recorder = new Recorder();
    m_player = new player();
    connect(m_player, &player::playEnd, this, &controller::playerFinished);
    connect(m_player, &player::playProgress, this, &controller::handlePlayProgress);
}

QString controller::getAudioFile() const
{
    return m_audioFile;
}

void controller::setAudioFile(const QString &value)
{
    m_audioFile = value;
}

void controller::handlePlayProgress(long long progress)
{
    int progressToInt = static_cast<int>(1000 - progress/1000);
    emit UpdatePlayProgress(progressToInt);
}

void controller::playerFinished()
{
    m_currentStatus = Stopped;
    emit NotifyStatusChange(m_currentStatus);
    emit UpdatePlayProgress(0);
}

