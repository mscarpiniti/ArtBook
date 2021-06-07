#include "mainwindow.h"

#include <QApplication>
#include "midi_synth.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.SetMidiSynthReference(new midi_synth());
    w.InitAll();

    w.show();
    return a.exec();
}
