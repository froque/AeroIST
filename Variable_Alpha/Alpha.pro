QT       += gui
QT       += xml

CONFIG         += plugin
CONFIG         += debug_and_release
TARGET          = $$qtLibraryTarget(alpha)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_real

INCLUDEPATH += ../common

SOURCES       += alpha.cpp
SOURCES       += ../common/botoneira.cpp \
                ../common/arduino-serial.cpp

HEADERS       += alpha.h
HEADERS       += ../common/botoneira.h \
                ../common/arduino-serial.h
