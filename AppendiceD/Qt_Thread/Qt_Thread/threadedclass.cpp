#include "threadedclass.h"

void ThreadedClass::run()
{
    static int l_counter = 0;
    while(m_threadMustRun)
    {
        // Qui va inserito il codice che deve essere eseguito dal thread secondario

        QString l_str = "Item number: " + QString::number(l_counter++);
        emit SendString(l_str);
        QThread::sleep(1);
    }
}

void ThreadedClass::StartStopThread()
{
    if (!this->isRunning())
    {
        m_threadMustRun = true;
        this->start();
        emit SendRunningStatus(true);
    }
    else
    {
        m_threadMustRun = false;
        this->wait();
        emit SendRunningStatus(false);
    }
}
