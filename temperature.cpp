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

//#include "arduino-serial.h"

#include <QSettings>

extern "C" int init_arduino(const char *port);
extern "C" int serialport_write(int fd, const char* str);
extern "C" int serialport_read_until(int fd, char* buf, char until);


Temperature::Temperature(){
    temp=0;
    temp_raw=0;
    QSettings settings;
    arduinofd = init_arduino(settings.value("arduino_path").toString().toStdString().c_str());
//    arduinofd = init_arduino(ARDUINOPATH);
}

Temperature::~Temperature(){
}




void Temperature::read(void){
    char buffer_read[256]="", buffer_aux[256];
    char buffer[] = "T0xxxx";
    bool sucess=false;

    sprintf(buffer_aux,"\"%s\"",buffer);
    while (sucess == false){
            if( serialport_write(arduinofd, buffer) == -1){
                perror("writing");
//	        return -1;
            }

            if( serialport_read_until(arduinofd, buffer_read, '\n') == -1){
                perror("reading");
//	        return -1;
            }
            if (strncmp(buffer_aux,buffer_read,3)==0){
            strncpy(buffer_aux,buffer_read + 3,4);
            temp_raw = atoi(buffer_aux);
            sucess=true;
        }
    }
    // 5V
    // 10 bits = 1024
    // 100 ºC/V = 0,01 V/ºC
    temp = temp_raw * 5.0 * 100.0 /1024.0;

}

