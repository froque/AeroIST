QT       += gui

CONFIG         += plugin
CONFIG         += debug_and_release
TARGET          = $$qtLibraryTarget(virtual_beta)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins

SOURCES       += virtual_beta.cpp

HEADERS       += virtual_beta.h

