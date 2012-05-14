QT       += gui
QT       += xml

CONFIG         += plugin
TARGET          = $$qtLibraryTarget(10_force)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_real

INCLUDEPATH += ../common
LIBS += -L/usr/local/lib -lgpib

SOURCES       += force.cpp

HEADERS       += force.h

