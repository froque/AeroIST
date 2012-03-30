QT       += gui
QT       += xml

CONFIG         += plugin
CONFIG         += debug_and_release
TARGET          = $$qtLibraryTarget(10_force)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_real

INCLUDEPATH += ../common

SOURCES       += force.cpp

HEADERS       += force.h

