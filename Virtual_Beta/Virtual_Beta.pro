QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(22_beta)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_virtual

INCLUDEPATH += ../common

SOURCES       += virtual_beta.cpp

HEADERS       += virtual_beta.h

