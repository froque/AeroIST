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
SOURCES += arduino-serial.cpp

HEADERS += botoneira.h
HEADERS += arduino-serial.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
