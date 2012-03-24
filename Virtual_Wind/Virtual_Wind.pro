QT       += gui
QT       += xml

CONFIG         += plugin
CONFIG         += debug_and_release
TARGET          = $$qtLibraryTarget(wind)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_virtual

INCLUDEPATH += ../common

SOURCES       += virtual_wind.cpp

HEADERS       += virtual_wind.h

