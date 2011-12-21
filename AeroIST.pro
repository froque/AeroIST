# -------------------------------------------------
# Project created by QtCreator 2011-08-11T23:59:52
# -------------------------------------------------
QT += core
QT += gui
QT += xml
QT += xmlpatterns

#TARGET = AeroIST
TEMPLATE = app

DESTDIR = build
OBJECTS_DIR = objs
MOC_DIR = moc
UI_DIR = ui
LIBS += -L/usr/local/lib -lgpib
CONFIG += qwt
#INCLUDEPATH += /usr/include/qwt
#CONFIG += 32bit
#CONFIG += 64bit
CONFIG(32bit) {
    LIBS += -L/usr/local/lib -lgpib
    TARGET = 32bit_binary
#    QMAKE_CXXFLAGS += -m32
#    QMAKE_CXX_FLAGS_RELEASE += -m32
#    LIBS += -L<path to 32bit libraries>
}
CONFIG(64bit) {
    LIBS += -L/usr/lib -lgpib
    TARGET = 64bit_binary
}
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
    curvedelete.cpp \
    force.cpp \
    angle.cpp \
    temperature.cpp \
    botoneira.cpp \
    arduino-serial.cpp \
    motor.cpp \
    wind.cpp \
    helper.cpp

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
    curvedelete.h \
    force.h \
    angle.h \
    temperature.h \
    botoneira.h \
    arduino-serial.h \
    motor.h \
    wind.h \
    helper.h

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
