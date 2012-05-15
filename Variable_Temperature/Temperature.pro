include(../common.pri)
QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(50_temperature)

TEMPLATE = lib

OBJECTS_DIR = $$BUILDDIR/objs/$$TARGET
MOC_DIR     = $$BUILDDIR/moc/$$TARGET
DESTDIR     = $$BUILDDIR/plugins_real

INCLUDEPATH += ../common

SOURCES       += temperature.cpp

HEADERS       += temperature.h


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../staticlibs/arduino-botoneira/release/ -larduino-serial
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../staticlibs/arduino-botoneira/debug/ -larduino-serial
else:symbian: LIBS += -lbotoneira
else:unix: LIBS += -L$$OUT_PWD/../staticlibs/arduino-botoneira/ -larduino-serial

INCLUDEPATH += $$PWD/../staticlibs/arduino-botoneira
DEPENDPATH += $$PWD/../staticlibs/arduino-botoneira

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/arduino-botoneira/release/arduino-serial.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/arduino-botoneira/debug/arduino-serial.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/arduino-botoneira/libarduino-serial.a

target.path = $$SHAREDIR/aeroist/plugins/
INSTALLS += target
