# -------------------------------------------------
# Project created by QtCreator 2011-08-11T23:59:52
# -------------------------------------------------


TEMPLATE = subdirs


SUBDIRS += aeroist/AeroIST.pro

# DESTDIR = plugins_real
SUBDIRS += \
    Variable_Force/Force.pro\
    Variable_Alpha/Alpha.pro\
    Variable_Beta/Beta.pro\
    Variable_Motor/Motor.pro\
    Variable_Temperature/Temperature.pro\
    Variable_Wind/Wind.pro\
    Variable_Angles/Angles.pro

# DESTDIR = plugins_virtual
SUBDIRS += \
    Virtual_Force\
    Virtual_Alpha\
    Virtual_Beta\
    Virtual_Motor\
    Virtual_Temperature\
    Virtual_Wind

# DESTDIR = plugins
SUBDIRS += Variable_Time/Time.pro
SUBDIRS += arduino/Arduino.pro

TRANSLATIONS = translations/aeroist_en.ts\
                translations/aeroist_pt.ts
