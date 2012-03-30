QT       += gui
QT       += xml

CONFIG         += plugin
CONFIG         += debug_and_release
TARGET          = $$qtLibraryTarget(00_time)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins

INCLUDEPATH += ../common

SOURCES       += time.cpp

HEADERS       += time.h

