#-------------------------------------------------
#
# Project created by QtCreator 2012-05-04T17:32:33
#
#-------------------------------------------------

QT       -= core gui

TARGET = botoneira
TEMPLATE = lib
CONFIG += staticlib

OBJECTS_DIR   = objs


SOURCES += botoneira.cpp

HEADERS += botoneira.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../arduino-serial/release/ -larduino-serial
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../arduino-serial/debug/ -larduino-serial
else:symbian: LIBS += -larduino-serial
else:unix: LIBS += -L$$OUT_PWD/../arduino-serial/ -larduino-serial

INCLUDEPATH += $$PWD/../arduino-serial
DEPENDPATH += $$PWD/../arduino-serial

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../arduino-serial/release/arduino-serial.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../arduino-serial/debug/arduino-serial.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../arduino-serial/libarduino-serial.a
