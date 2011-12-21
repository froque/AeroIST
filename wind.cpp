#include "wind.h"

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

Wind::Wind(){
    wind=0;
    wind_raw=0;
    QSettings settings;
    arduinofd = serialport_init(settings.value(SETTINGS_ARDUINO_PATH).toString().toStdString().c_str(),SERIALRATE);
}

Wind::~Wind(){
    close(arduinofd);
}

void Wind::read(void){
    char buffer_read[256]="", buffer_aux[256];
    char buffer[] = "A5xxxx";               // read from analog 5
    bool sucess=false;

    sprintf(buffer_aux,"\"%s\"",buffer);
    while (sucess == false){
        serialport_flush(arduinofd);
        if( serialport_write(arduinofd, buffer) == -1){
            perror("Wind writing");
        }

        serialport_read_until(arduinofd, buffer_read, '\n');

        if (strncmp(buffer_aux,buffer_read,3)==0){
            strncpy(buffer_aux,buffer_read + 3,4);
            wind_raw = atoi(buffer_aux);
            sucess=true;
        }
    }
    // 3.3V
    // 10 bits = 1024
    // 19.9 mm H20 = 5 V => 3.98 mm H20/V
    // 1 mm H20 = 9.80665 Pa
//    wind = wind_raw * ARDUINO_ANALOG_REF/1024.0 * WIND_SENSITIVITY * MMH2O_TO_PASCAL ;
    wind = wind_raw * ARDUINO_ANALOG_REF/1024.0 * WIND_SENSITIVITY ;
}
