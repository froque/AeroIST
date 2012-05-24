# -------------------------------------------------
# Project created by QtCreator 2011-08-11T23:59:52
# -------------------------------------------------
include(common.pri)
PLUGINS = real

TEMPLATE = subdirs

CONFIG += ordered

# internal libs for static compilation
SUBDIRS += staticlibs/staticlib.pro


SUBDIRS += aeroist/AeroIST.pro

# DESTDIR = plugins_real
contains(PLUGINS,real){
SUBDIRS += \
    Variable_Force/Force.pro\
    Variable_Alpha/Alpha.pro\
    Variable_Beta/Beta.pro\
    Variable_Motor/Motor.pro\
    Variable_Temperature/Temperature.pro\
    Variable_Wind/Wind.pro\
    Variable_Angles/Angles.pro
}
# DESTDIR = plugins_virtual
contains(PLUGINS,virtual){
SUBDIRS += \
    Virtual_Force\
    Virtual_Alpha\
    Virtual_Beta\
    Virtual_Motor\
    Virtual_Temperature\
    Virtual_Wind
}

# DESTDIR = plugins
SUBDIRS += Variable_Time/Time.pro
SUBDIRS += arduino/Arduino.pro



TRANSLATIONS = translations/aeroist_en.ts\
                translations/aeroist_pt.ts

schema.files       = resources/aeroist.xsd
schema.path        = $$SHAREDIR/aeroist/
matrices.files     = "resources/matrix 1.coe" "resources/matrix 2.coe"
matrices.path      = $$SHAREDIR/aeroist/
translations.files = translations/aeroist_en.qm translations/aeroist_pt.qm
translations.path  = $$SHAREDIR/aeroist/
udev.files         = resources/40-libgpib-bin.rules resources/50-usb-serial.rules
udev.path          = $$UDEVDIR/
dotdesktop.files   = resources/aeroist.desktop
dotdesktop.path    = $$DOTDESKTOPDIR/
icons.files        = aeroist/resources/aeroist.svg
icons.path         = $$ICONDIR/
INSTALLS += schema matrices translations udev dotdesktop icons
