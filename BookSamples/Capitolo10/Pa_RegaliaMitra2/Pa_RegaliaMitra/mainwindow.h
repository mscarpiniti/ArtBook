#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "streamcontroller.h"
#include <QCloseEvent>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    StreamController* m_streamController;
    RegMitraParameters* m_filterParamsLocal;
    OscillatorParameters* m_leftOscParamsLocal;
    OscillatorParameters* m_rightOscParamsLocal;
    InputParameters* m_inputParamsLocal;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    virtual void closeEvent (QCloseEvent *event);
    QString HertzToString(double);

public slots:
    //necessario per gestire le segnalazioni dallo streamer
    void OnStreamingStatusChange(StreamStatus);
    void StartStopStream();

    //necessari per gestire gli eventi dall'interfaccia grafica
    void OnFilterChangeAbilitation(int);
    void OnFilterChangeCutoff(double);
    void OnFilterChangeLevel(double);

    void OnInputChangeAbilitation(int);
    void OnInputFilterSignal(int);
    void OnInputChangeLevel(double);

    void OnLeftOscLevel(double);
    void OnLeftOscFreq(double);
    void OnLeftOscAttiv(int);
    void OnLeftOscFilterSig(int);

    void OnRightOscLevel(double);
    void OnRightOscFreq(double);
    void OnRightOscAttiv(int);
    void OnRightOscFilterSig(int);
};

#endif // MAINWINDOW_H
