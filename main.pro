# -------------------------------------------------
# Project created by QtCreator 2011-08-11T23:59:52
# -------------------------------------------------


TEMPLATE = subdirs


SUBDIRS += \
    AeroIST.pro

SUBDIRS += \
    Variable_Force/Force.pro\
#    Variable_Alpha/Alpha.pro\
    Variable_Beta/Beta.pro\
    Variable_Motor/Motor.pro\
    Variable_Temperature/Temperature.pro\
    Variable_Wind/Wind.pro\
    Variable_Time/Time.pro\
    Variable_Angles/Angles.pro

SUBDIRS += \
    Virtual_Force\
    Virtual_Alpha\
    Virtual_Beta\
    Virtual_Motor\
    Virtual_Temperature\
    Virtual_Wind\
    Virtual_Time\

SUBDIRS += arduino/Arduino.pro
