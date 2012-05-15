include(../common.pri)
QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(10_force)

TEMPLATE = lib

OBJECTS_DIR = $$BUILDDIR/objs/$$TARGET
MOC_DIR     = $$BUILDDIR/moc/$$TARGET
DESTDIR     = $$BUILDDIR/plugins_real

INCLUDEPATH += ../common
LIBS += -L/usr/local/lib -lgpib

SOURCES       += force.cpp

HEADERS       += force.h

target.path = $$SHAREDIR/aeroist/plugins/
INSTALLS += target
