#ifndef THREADEDCLASS_H
#define THREADEDCLASS_H

#include <QThread>

class ThreadedClass : public QThread
{
    Q_OBJECT

private:
    bool m_threadMustRun;

    void run() Q_DECL_OVERRIDE;
public:

signals:
    void SendString(QString);
    void SendRunningStatus(bool);
public slots:
    void StartStopThread();

};

#endif // THREADEDCLASS_H
