QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(arduino)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins

INCLUDEPATH += ../common

SOURCES       += arduino.cpp

HEADERS       += arduino.h
