#-------------------------------------------------
#
# Project created by QtCreator 2017-02-04T14:20:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pa_RegaliaMitra
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    streamcontroller.cpp

HEADERS  += mainwindow.h \
    streamcontroller.h \
    audioio.h

FORMS    += mainwindow.ui


INCLUDEPATH += $$PWD/../../../../source/portaudio/include
DEPENDPATH += $$PWD/../../../../source/portaudio/include

macx {
    LIBS += -L$$PWD/../../../../source/portaudio/lib/.libs/ -lportaudio
}

win32 {
    LIBS += -L$$PWD/../../../../source/portaudio/lib/.libs/ -lportaudio
}

unix:!macx {
    LIBS += -L$$PWD/../../../../source/portaudio/lib/.libs/ -lportaudio
    PRE_TARGETDEPS += $$PWD/../../../../source/portaudio/lib/.libs/libportaudio.a
    LIBS += -lasound
}
