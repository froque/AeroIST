QT       += gui
QT       += xml

CONFIG         += plugin
CONFIG         += debug_and_release
TARGET          = $$qtLibraryTarget(alpha)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_virtual

INCLUDEPATH += ../common

SOURCES       += virtual_alpha.cpp

HEADERS       += virtual_alpha.h

