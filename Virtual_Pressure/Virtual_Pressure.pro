include(../common.pri)
QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(40_pressure)

TEMPLATE = lib

OBJECTS_DIR = $$BUILDDIR/objs/$$TARGET
MOC_DIR     = $$BUILDDIR/moc/$$TARGET
DESTDIR     = $$BUILDDIR/plugins_virtual

INCLUDEPATH += ../common

SOURCES       += virtual_pressure.cpp

HEADERS       += virtual_pressure.h

