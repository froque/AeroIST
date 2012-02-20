QT       += gui
QT       += xml

CONFIG         += plugin
CONFIG         += debug_and_release
TARGET          = $$qtLibraryTarget(beta)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_real

SOURCES       += beta.cpp \
                ../botoneira.cpp \
                ../arduino-serial.cpp

HEADERS       += beta.h \
                ../botoneira.h \
                ../arduino-serial.h


