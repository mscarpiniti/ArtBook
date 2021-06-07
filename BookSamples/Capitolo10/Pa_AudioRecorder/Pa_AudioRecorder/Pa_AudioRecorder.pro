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
    controller.cpp \
    main.cpp \
    mainwindow.cpp \
    player.cpp \
    recorder.cpp

HEADERS += \
    controller.h \
    mainwindow.h \
    audioio.h \
    player.h \
    recorder.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/../../../../source/portaudio/include
DEPENDPATH += $$PWD/../../../../source/portaudio/include
INCLUDEPATH += $$PWD/../../../../source/libsndfile/src
DEPENDPATH += $$PWD/../../../../source/libsndfile/src


macx {
    LIBS += -L$$PWD/../../../../source/portaudio/lib/.libs/ -lportaudio
    LIBS += -L$$PWD/../../../../source/libsndfile/src/.libs/ -lsndfile
}

win32 {
    LIBS += -L$$PWD/../../../../source/portaudio/lib/.libs/ -llibportaudio.dll
    LIBS += -L$$PWD/../../../../source/libsndfile/src/.libs/ -lsndfile
}

unix:!macx {
    LIBS += -L$$PWD/../../../../source/libsndfile/src/.libs/ -lsndfile
    PRE_TARGETDEPS += $$PWD/../../../../source/libsndfile/src/.libs/libsndfile.a
    LIBS += -L$$PWD/../../../../source/portaudio/lib/.libs/ -lportaudio
    PRE_TARGETDEPS += $$PWD/../../../../source/portaudio/lib/.libs/libportaudio.a
    LIBS += -lasound
}


RESOURCES += \
    Resources.qrc


