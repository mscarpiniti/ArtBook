QT += core
QT -= gui

CONFIG += c++11

TARGET = Suona_SMF
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    seq2midi.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH += $$PWD/../../../../source/portsmf
DEPENDPATH += $$PWD/../../../../source/portsmf

INCLUDEPATH += $$PWD/../../../../source/portmidi/pm_common
DEPENDPATH += $$PWD/../../../../source/portmidi/pm_common

INCLUDEPATH += $$PWD/../../../../source/portmidi/porttime
DEPENDPATH += $$PWD/../../../../source/portmidi/porttime


win32 {
    LIBS += -L$$PWD/../../../../source/portsmf/ -lportSMF
    LIBS += -L$$PWD/../../../../source/portmidi_build/release/ -lportmidi
}

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../../../source/portsmf/portSMF.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../../../../source/portsmf/libportSMF.a


macx {
    LIBS += -L$$PWD/../../../../source/portsmf/ -lportSMF
    PRE_TARGETDEPS += $$PWD/../../../../source/portsmf/libportSMF.a
    LIBS += -L$$PWD/../../../../source/portmidi_build/release/ -lportmidi
    PRE_TARGETDEPS += $$PWD/../../../../source/portmidi_build/release/libportmidi.a
    LIBS += -framework CoreAudio -framework CoreFoundation -framework CoreMidi -framework CoreData
}

unix:!macx {
    LIBS += -L$$PWD/../../../../source/portsmf/ -lportSMF
    PRE_TARGETDEPS += $$PWD/../../../../source/portsmf/libportSMF.a
    LIBS += -L$$PWD/../../../../source/portmidi_build/release/ -lportmidi
    PRE_TARGETDEPS += $$PWD/../../../../source/portmidi_build/release/libportmidi.a
}
