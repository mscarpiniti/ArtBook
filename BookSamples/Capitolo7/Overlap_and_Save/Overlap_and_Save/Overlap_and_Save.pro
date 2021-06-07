QT += core
QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#TEMPLATE = app

SOURCES += main.cpp

HEADERS += audioIO.h

#DEFINES += QT_DEPRECATED_WARNINGS


INCLUDEPATH += $$PWD/../../../../source/portaudio/include
DEPENDPATH += $$PWD/../../../../source/portaudio/include

INCLUDEPATH += $$PWD/../../../../source/fftw/api
DEPENDPATH += $$PWD/../../../../source/fftw/api


macx {
    LIBS += -L$$PWD/../../../../source/portaudio/lib/.libs/ -lportaudio
    LIBS += -L$$PWD/../../../../source/fftw/.libs/ -lfftw3
}

win32 {
    LIBS += -L$$PWD/../../../../source/portaudio/lib/.libs/ -lportaudio
    LIBS += -L$$PWD/../../../../source/fftw/.libs/ -lfftw3
}

unix:!macx {
    LIBS += -L$$PWD/../../../../source/portaudio/lib/.libs/ -lportaudio
    PRE_TARGETDEPS += $$PWD/../../../../source/portaudio/lib/.libs/libportaudio.a
    LIBS += -lasound
    LIBS += -L$$PWD/../../../../source/fftw/.libs/ -lfftw3
    PRE_TARGETDEPS += $$PWD/../../../../source/fftw/.libs/libfftw3.a
}
