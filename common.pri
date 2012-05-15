
UDEVDIR    = /etc/udev/rules.d

isEmpty(PREFIX) {
        PREFIX = /usr/local
}

SHAREDIR   = $$PREFIX/share
SYMLINKDIR = $$PREFIX/bin

BUILDDIR    = ../build
