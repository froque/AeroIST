QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(50_temperature)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_virtual

INCLUDEPATH += ../common

SOURCES       += virtual_temperature.cpp

HEADERS       += virtual_temperature.h

