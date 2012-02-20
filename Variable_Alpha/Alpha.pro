QT       += gui
QT       += xml

CONFIG         += plugin
CONFIG         += debug_and_release
TARGET          = $$qtLibraryTarget(alpha)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_real

SOURCES       += alpha.cpp \
                ../botoneira.cpp \
                ../arduino-serial.cpp

HEADERS       += alpha.h \
                ../botoneira.h \
                ../arduino-serial.h
