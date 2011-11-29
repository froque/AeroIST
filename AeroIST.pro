# -------------------------------------------------
# Project created by QtCreator 2011-08-11T23:59:52
# -------------------------------------------------
QT += core \
    gui
TARGET = AeroIST
TEMPLATE = app

DESTDIR = build
OBJECTS_DIR = objs
MOC_DIR = moc
UI_DIR = ui


SOURCES += main.cpp \
    aeroistwindow.cpp \
    measurementsmodel.cpp \
    measurethread.cpp \
#    timescaledraw.cpp \
    preferences.cpp \
    measurelist.cpp \
    measurementspreferences.cpp \
    measurementdetails.cpp
#    zerosmodel.cpp

HEADERS += aeroistwindow.h \
    qwt_plot.h \
    measure.h \
    measurementsmodel.h \
    measurethread.h \
#    timescaledraw.h \
    preferences.h \
    measurelist.h \
    measurementspreferences.h \
    common.h \
    measurementdetails.h
#    zerosmodel.h

FORMS += aeroistwindow.ui \
    preferences.ui \
    measurementspreferences.ui \
    measurementdetails.ui

TRANSLATIONS = aeroist_en.ts\
                aeroist_pt.ts


LIBS += -lqwt #-LC:/usr/lib/libqwt-qt4.so
#LIBS += -lqwt-qt4 -LC:/usr/lib/libqwt-qt4.so

#QMAKESPEC=linux-g++-64
#QMAKESPEC=linux-g++-32
#CONFIG += x86
#CONFIG -= x86_64
