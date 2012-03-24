# -------------------------------------------------
# Project created by QtCreator 2011-08-11T23:59:52
# -------------------------------------------------
QT += core
QT += gui
QT += xml
QT += xmlpatterns

#TARGET = AeroIST
TEMPLATE = app

DESTDIR = ../build
OBJECTS_DIR = $$DESTDIR/objs
MOC_DIR = $$DESTDIR/moc
UI_DIR = $$DESTDIR/ui
RCC_DIR = $$DESTDIR/rcc

INCLUDEPATH += ../common
LIBS += -L/usr/local/lib -lgpib
CONFIG += qwt

SOURCES += main.cpp \
    aeroistwindow.cpp \
    measurementsmodel.cpp \
    measurethread.cpp \
    preferences.cpp \
    measurelist.cpp \
    measurementspreferences.cpp \
    measurementdetails.cpp \
    curvenew.cpp \
    curvedelete.cpp \
    referencedetails.cpp \
    referencelist.cpp \
    referencemodel.cpp \
    referencepreferences.cpp

SOURCES += ../common/helper.cpp \

HEADERS += aeroistwindow.h \
    measurementsmodel.h \
    measurethread.h \
    preferences.h \
    measurelist.h \
    measurementspreferences.h \
    common.h \
    measurementdetails.h \
    curvenew.h \
    curvedelete.h \
    referencedetails.h \
    referencelist.h \
    referencemodel.h \
    referencepreferences.h

HEADERS += ../common/variable.h \
    ../common/helper.h \

FORMS += aeroistwindow.ui \
    preferences.ui \
    measurementspreferences.ui \
    measurementdetails.ui \
    deleteplot.ui \
    curvenew.ui \
    referencedetails.ui \
    referencepreferences.ui

TRANSLATIONS = aeroist_en.ts\
                aeroist_pt.ts

RESOURCES += resources.qrc
