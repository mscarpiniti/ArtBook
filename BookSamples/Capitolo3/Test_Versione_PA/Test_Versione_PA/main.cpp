#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "portaudio.h"

using namespace std;


int main(int argc, char *argv[])
{    
    PaError err = Pa_Initialize();
    if( err == paNoError )
    {
        cout << "Versione di PortAudio: " << Pa_GetVersion() << endl;
        cout << "Info versione: " << Pa_GetVersionInfo()->versionText << endl;
    }
    else
    {

        cout << "Codice di errore: " << err << endl;
        cout << "Messaggio di errore: " << Pa_GetErrorText(err) << endl;
    }

    int versione;
    QString release;

    versione = Pa_GetVersion();
    release = Pa_GetVersionInfo()->versionText;

    Pa_Terminate();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.SetVersion(versione);
    w.SetInfo(&release);
    a.setWindowIcon(QIcon("portaudio_logo.png"));

    return a.exec();
}
