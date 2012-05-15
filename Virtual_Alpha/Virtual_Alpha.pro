include(../common.pri)
QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(21_alpha)

TEMPLATE = lib

OBJECTS_DIR = $$BUILDDIR/objs/$$TARGET
MOC_DIR     = $$BUILDDIR/moc/$$TARGET
DESTDIR     = $$BUILDDIR/plugins_virtual

INCLUDEPATH += ../common

SOURCES       += virtual_alpha.cpp

HEADERS       += virtual_alpha.h

