QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(30_motor)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_real

INCLUDEPATH += ../common

SOURCES       += motor.cpp \
                ../common/helper.cpp

HEADERS       += motor.h \
                ../common/helper.h

