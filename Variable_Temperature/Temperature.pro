QT       += gui
QT       += xml

CONFIG         += plugin
CONFIG         += debug_and_release
TARGET          = $$qtLibraryTarget(50_temperature)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_real

INCLUDEPATH += ../common

SOURCES       += temperature.cpp

HEADERS       += temperature.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../staticlibs/arduino-serial/release/ -larduino-serial
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../staticlibs/arduino-serial/debug/ -larduino-serial
else:symbian: LIBS += -larduino-serial
else:unix: LIBS += -L$$OUT_PWD/../staticlibs/arduino-serial/ -larduino-serial

INCLUDEPATH += $$PWD/../staticlibs/arduino-serial
DEPENDPATH += $$PWD/../staticlibs/arduino-serial

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/arduino-serial/release/arduino-serial.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/arduino-serial/debug/arduino-serial.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/arduino-serial/libarduino-serial.a
