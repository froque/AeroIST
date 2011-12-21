#include "temperature.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>   /* Standard types */
#include "common.h"
#include "arduino-serial.h"

#include <QSettings>

Temperature::Temperature(){
    temp=0;
    temp_raw=0;
    QSettings settings;
    arduinofd = serialport_init(settings.value(SETTINGS_ARDUINO_PATH).toString().toStdString().c_str(),SERIALRATE);
}

Temperature::~Temperature(){
    close(arduinofd);
}

void Temperature::read(void){
    char buffer_read[256]="", buffer_aux[256];
    char buffer[] = "T0xxxx";
    bool sucess=false;

    sprintf(buffer_aux,"\"%s\"",buffer);
    while (sucess == false){
        serialport_flush(arduinofd);
        if( serialport_write(arduinofd, buffer) == -1){
            perror("temperature writing");
        }

        serialport_read_until(arduinofd, buffer_read, '\n');

        if (strncmp(buffer_aux,buffer_read,3)==0){
            strncpy(buffer_aux,buffer_read + 3,4);
            temp_raw = atoi(buffer_aux);
            sucess=true;
        }
    }
    // 3.3V
    // 10 bits = 1024
    // 100 ºC/V = 0,01 V/ºC
    temp = temp_raw * ARDUINO_ANALOG_REF * TEMPERATURE_SENSITIVITY /1024.0;
}

