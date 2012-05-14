QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(40_wind)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_virtual

INCLUDEPATH += ../common

SOURCES       += virtual_wind.cpp

HEADERS       += virtual_wind.h

