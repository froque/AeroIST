QT       += gui
QT       += xml

CONFIG         += plugin
CONFIG         += debug_and_release
TARGET          = $$qtLibraryTarget(angles)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_real

SOURCES       += angles.cpp \
                ../botoneira.cpp \
                ../arduino-serial.cpp

HEADERS       += angles.h \
                ../botoneira.h \
                ../arduino-serial.h

INCLUDEPATH += ../qserialdevice/include

HEADERS += \
    ../qserialdevice/include/serialport.h \
    ../qserialdevice/include/serialportinfo.h

HEADERS += \
    ../qserialdevice/src/serialport_p.h \
    ../qserialdevice/src/ringbuffer_p.h \
    ../qserialdevice/src/serialportengine_p.h \
    ../qserialdevice/src/serialportinfo_p.h

SOURCES += \
    ../qserialdevice/src/serialport.cpp \
    ../qserialdevice/src/serialportinfo.cpp

win32 {
    HEADERS += \
        ../qserialdevice/src/serialportengine_p_win.h
    SOURCES += \
        ../qserialdevice/src/serialportengine_p_win.cpp \
        ../qserialdevice/src/serialportinfo_win.cpp

    !wince*: LIBS += -lsetupapi -luuid -ladvapi32
}
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target

    HEADERS += \
        ../qserialdevice/src/ttylocker_p_unix.h \
        ../qserialdevice/src/serialportengine_p_unix.h
    SOURCES += \
        ../qserialdevice/src/ttylocker_p_unix.cpp \
        ../qserialdevice/src/serialportengine_p_unix.cpp

    macx {
        SOURCES += ../qserialdevice/src/serialportinfo_mac.cpp
        LIBS += -framework IOKit -framework CoreFoundation
    } else {
        SOURCES += ../qserialdevice/src/serialportinfo_unix.cpp
        linux*:contains( DEFINES, HAVE_LIBUDEV ) {
            LIBS += -ludev
        }
    }
}
