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

CONFIG += qwt

#QMAKESPEC=linux-g++-64
#QMAKESPEC=linux-g++-32
#CONFIG += x86
#CONFIG -= x86_64

SOURCES += main.cpp \
    aeroistwindow.cpp \
    measurementsmodel.cpp \
    measurethread.cpp \
    preferences.cpp \
    measurelist.cpp \
    measurementspreferences.cpp \
    measurementdetails.cpp \
    zeromodel.cpp \
    zerolist.cpp \
    zerodetails.cpp \
    zeropreferences.cpp \
    curvenew.cpp \
    curvedelete.cpp

HEADERS += aeroistwindow.h \
    measure.h \
    measurementsmodel.h \
    measurethread.h \
    preferences.h \
    measurelist.h \
    measurementspreferences.h \
    common.h \
    measurementdetails.h \
    zeromodel.h \
    zerolist.h \
    zerodetails.h \
    zeropreferences.h \
    curvenew.h \
    curvedelete.h

FORMS += aeroistwindow.ui \
    preferences.ui \
    measurementspreferences.ui \
    measurementdetails.ui \
    zerodetails.ui \
    zeropreferences.ui \
    newcurve.ui \
    deleteplot.ui

TRANSLATIONS = aeroist_en.ts\
                aeroist_pt.ts
