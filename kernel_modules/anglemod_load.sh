#!/bin/bash
DEVICENAMES="/dev/angle_"
#DEVICENAMES="angle_"
MODULE="anglemod"
MODE="664"
USER="flip"

function insert_module {
        if (( $(lsmod|grep $MODULE|wc -l) == 1)); then
            echo "removing module"
        	/sbin/rmmod $MODULE
        fi
    	/sbin/insmod -f ./$MODULE.ko $* || exit 1
    	MAJOR=$(cat /proc/devices | awk "\$2==\"$MODULE\" {print \$1}")
    	rm -f ${DEVICENAMES}*
    	mknod ${DEVICENAMES}beta c $MAJOR 0
	    mknod ${DEVICENAMES}alpha c $MAJOR 1
    	chown $USER: ${DEVICENAMES}*
        echo "module inserted"
}


if [[ $(id -u) -ne 0 ]]; then
	echo "not root user"
elif /usr/bin/make -q; then
    echo "not updated"
    /usr/bin/make;
    insert_module;
else
    echo "updated"
    insert_module;
#	    echo "error on make"
#    	exit
#    fi
fi


