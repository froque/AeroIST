CONFIG += ordered

TEMPLATE = subdirs

unix:SUBDIRS += arduino-botoneira/arduino-serial.pro
unix:SUBDIRS += arduino-botoneira/botoneira.pro
SUBDIRS += qserialdevice/src/src.pro
