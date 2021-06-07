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
    InputParameters* m_inputParamsLocal;
    Ui::MainWindow *ui;
    virtual void closeEvent (QCloseEvent *event);
    QString HertzToString(double);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    //necessario per gestire le segnalazioni dallo streamer
    void OnStreamingStatusChange(StreamStatus);
    void StartStopStream();

    //necessari per gestire gli eventi dall'interfaccia grafica
    void OnFilterChangeAbilitation(int);
    void OnFilterChangeCutoff(double);
    void OnFilterChangeLevel(double);
    void OnInputChangeLevel(double);

};

#endif // MAINWINDOW_H
