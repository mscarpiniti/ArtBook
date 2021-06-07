#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <controller.h>
#include <QPushButton>
#include <QMovie>
#include <QLabel>
#include <QProgressBar>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    controller* m_theController;
    QMovie *m_recAnim, *m_playAnim;
    QLabel *m_recordLabel;
    QProgressBar* m_playProgress;

    void SetIcon(QPushButton*, QString, QString);


public slots:

    void PlayBtnPressed();
    void RecordBtnPressed();
    void HandleStatusChange(State);
    void SetProgress(int);
    QString OpenFile(openMode);

};
#endif // MAINWINDOW_H
