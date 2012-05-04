QT       += gui
QT       += xml

CONFIG         += plugin
CONFIG         += debug_and_release
TARGET          = $$qtLibraryTarget(22_beta)

TEMPLATE = lib

OBJECTS_DIR   = objs
MOC_DIR       = moc
DESTDIR       = ../build/plugins_real

INCLUDEPATH += ../common

SOURCES       += beta.cpp

HEADERS       += beta.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../staticlibs/botoneira/release/ -lbotoneira
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../staticlibs/botoneira/debug/ -lbotoneira
else:symbian: LIBS += -lbotoneira
else:unix: LIBS += -L$$OUT_PWD/../staticlibs/botoneira/ -lbotoneira

INCLUDEPATH += $$PWD/../staticlibs/botoneira
DEPENDPATH += $$PWD/../staticlibs/botoneira

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/botoneira/release/botoneira.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/botoneira/debug/botoneira.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../staticlibs/botoneira/libbotoneira.a
