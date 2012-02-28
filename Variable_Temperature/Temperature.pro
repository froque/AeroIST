QT       += gui
QT       += xml

CONFIG         += plugin
CONFIG         += debug_and_release
TARGET          = $$qtLibraryTarget(temperature)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_real

SOURCES       += temperature.cpp

HEADERS       += temperature.h

SOURCES       += ../arduino-serial.cpp
HEADERS       += ../arduino-serial.hn
