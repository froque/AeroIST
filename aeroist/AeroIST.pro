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

unix:CONFIG += qwt
win32:INCLUDEPATH += "C:/Qwt-6.0.2-svn/include"
win32:LIBS += -L"C:/Qwt-6.0.2-svn/lib" -lqwt

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
    referencepreferences.cpp \
    proxymodel.cpp

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
    referencepreferences.h \
    proxymodel.h

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

RESOURCES += resources.qrc

### FOR INSTALLATION
## Installation Variables
#isEmpty(PREFIX) {
#        PREFIX = /usr/local
#}
#BINDIR = $$PREFIX/bin

##MAKE INSTALL
#INSTALLS += target
#target.path = $$BINDIR