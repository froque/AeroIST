QT       += gui

CONFIG         += plugin
CONFIG         += debug_and_release
TARGET          = $$qtLibraryTarget(virtual_wind)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins

SOURCES       += virtual_wind.cpp

HEADERS       += virtual_wind.h

