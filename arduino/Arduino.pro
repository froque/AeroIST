include(../common.pri)
QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(arduino)

TEMPLATE = lib

OBJECTS_DIR = $$BUILDDIR/objs/$$TARGET
MOC_DIR     = $$BUILDDIR/moc/$$TARGET
DESTDIR     = $$BUILDDIR/plugins

INCLUDEPATH += ../common

SOURCES       += arduino.cpp

HEADERS       += arduino.h

target.path = $$SHAREDIR/aeroist/plugins/
INSTALLS += target
