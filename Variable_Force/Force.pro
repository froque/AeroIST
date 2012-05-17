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
unix:LIBS += -L/usr/local/lib -lgpib
win32:INCLUDEPATH += "C:/Program Files/National Instruments/Shared/ExternalCompilerSupport/C/include"
win32:LIBS += "C:/Program Files/National Instruments/Shared/ExternalCompilerSupport/C/lib32/msvc/ni4882.obj"

SOURCES       += force.cpp

HEADERS       += force.h

target.path = $$SHAREDIR/aeroist/plugins/
INSTALLS += target
