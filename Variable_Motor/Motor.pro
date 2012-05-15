include(../common.pri)
QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(30_motor)

TEMPLATE = lib

OBJECTS_DIR = $$BUILDDIR/objs/$$TARGET
MOC_DIR     = $$BUILDDIR/moc/$$TARGET
DESTDIR     = $$BUILDDIR/plugins_real

INCLUDEPATH += ../common

SOURCES       += motor.cpp \
                ../common/helper.cpp

HEADERS       += motor.h \
                ../common/helper.h

target.path = $$SHAREDIR/aeroist/plugins/
INSTALLS += target
