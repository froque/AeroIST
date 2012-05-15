include(../common.pri)
QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(10_force)

TEMPLATE = lib

OBJECTS_DIR = $$BUILDDIR/objs/$$TARGET
MOC_DIR     = $$BUILDDIR/moc/$$TARGET
DESTDIR     = $$BUILDDIR/plugins_virtual

INCLUDEPATH += ../common

SOURCES       += virtual_force.cpp

HEADERS       += virtual_force.h

