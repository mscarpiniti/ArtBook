#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "midi_synth.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void SetMidiSynthReference(midi_synth*);
    void InitAll();
    ~MainWindow();

private:
    Ui::MainWindow *m_ui;
    midi_synth* m_theSynth;

public slots:
    void SetMidiInput(const QString &);
    void SetFreq(float);
};
#endif // MAINWINDOW_H
