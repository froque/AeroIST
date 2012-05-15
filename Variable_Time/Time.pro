include(../common.pri)
QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(00_time)

TEMPLATE = lib

OBJECTS_DIR = $$BUILDDIR/objs/$$TARGET
MOC_DIR     = $$BUILDDIR/moc/$$TARGET
DESTDIR     = $$BUILDDIR/plugins

INCLUDEPATH += ../common

SOURCES       += time.cpp

HEADERS       += time.h

target.path = $$SHAREDIR/aeroist/plugins/
INSTALLS += target
