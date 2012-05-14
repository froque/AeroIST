QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(10_force)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_virtual

INCLUDEPATH += ../common

SOURCES       += virtual_force.cpp

HEADERS       += virtual_force.h

