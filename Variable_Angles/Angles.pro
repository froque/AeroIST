QT       += gui
QT       += xml

CONFIG         += plugin
CONFIG         += debug_and_release
TARGET          = $$qtLibraryTarget(20_angles)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_real

SOURCES       += angles.cpp

HEADERS       += angles.h

INCLUDEPATH += ../common



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../staticlibs/arduino-botoneira/release/ -lbotoneira
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../staticlibs/arduino-botoneira/debug/ -lbotoneira
else:symbian: LIBS += -lbotoneira
else:unix: LIBS += -L$$OUT_PWD/../staticlibs/arduino-botoneira/ -lbotoneira

INCLUDEPATH += $$PWD/../staticlibs/arduino-botoneira
DEPENDPATH += $$PWD/../staticlibs/arduino-botoneira

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/arduino-botoneira/release/botoneira.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/arduino-botoneira/debug/botoneira.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/arduino-botoneira/libbotoneira.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../staticlibs/qserialdevice/src/release/ -lSerialPort
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../staticlibs/qserialdevice/src/debug/ -lSerialPort
else:symbian: LIBS += -lSerialPort
else:unix: LIBS += -L$$OUT_PWD/../staticlibs/qserialdevice/src/ -lSerialPort

INCLUDEPATH += $$PWD/../staticlibs/qserialdevice/include
DEPENDPATH += $$PWD/../staticlibs/qserialdevice/src

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/qserialdevice/src/release/SerialPort.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/qserialdevice/src/debug/SerialPort.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/qserialdevice/src/libSerialPort.a
