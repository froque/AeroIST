QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(30_motor)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_virtual

INCLUDEPATH += ../common

SOURCES       += virtual_motor.cpp

HEADERS       += virtual_motor.h

