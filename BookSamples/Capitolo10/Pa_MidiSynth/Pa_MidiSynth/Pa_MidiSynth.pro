QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adsr.cpp \
    main.cpp \
    mainwindow.cpp \
    midi_event_handler.cpp \
    midi_synth.cpp \
    voice.cpp \
    voicebank.cpp

HEADERS += \
    adsr.h \
    mainwindow.h \
    midi_event_handler.h \
    midi_synth.h \
    voice.h \
    voicebank.h

INCLUDEPATH += $$PWD/../../../../source/portaudio/include
DEPENDPATH += $$PWD/../../../../source/portaudio/include
INCLUDEPATH += $$PWD/../../../../source/portmidi/pm_common
DEPENDPATH += $$PWD/../../../../source/portmidi/pm_common
INCLUDEPATH += $$PWD/../../../../source/portmidi/porttime
DEPENDPATH += $$PWD/../../../../source/portmidi/porttime

win32 {
    LIBS += -L$$PWD/../../../../source/portaudio/lib/.libs/ -llibportaudio.dll
    LIBS += -L$$PWD/../../../../source/portmidi_build/release/ -lportmidi
}

macx {
    LIBS += -L$$PWD/../../../../source/portaudio/lib/.libs/ -lportaudio
    LIBS += -L$$PWD/../../../../source/portmidi_build/release/ -lportmidi
    PRE_TARGETDEPS += $$PWD/../../../../source/portmidi_build/release/libportmidi.a
    LIBS += -framework CoreAudio -framework CoreFoundation -framework CoreMidi -framework CoreData
}

unix:!macx {
    LIBS += -L$$PWD/../../../../source/portaudio/lib/.libs/ -lportaudio
    PRE_TARGETDEPS += $$PWD/../../../../source/portaudio/lib/.libs/libportaudio.a
    LIBS += -L$$PWD/../../../../source/portmidi_build/release/ -lportmidi
    PRE_TARGETDEPS += $$PWD/../../../../source/portmidi_build/release/libportmidi.a
    LIBS += -lasound

}

FORMS += \
    mainwindow.ui




# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target












