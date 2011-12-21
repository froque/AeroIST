#include "angle.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <QSettings>
#include <QDebug>

#include "botoneira.h"


Angle::Angle(){
    QSettings settings;
    arduinofd = init_arduino(settings.value(SETTINGS_ARDUINO_PATH).toString().toStdString().c_str());
}

Angle::~Angle(){
    close(fp);
    close(arduinofd);
}

void Angle::set(double angle_dest){
    //max
    if (angle_dest > anglemax){
        angle_dest = anglemax;
    }
    //min
    if (angle_dest < -anglemax){
        angle_dest = -anglemax;
    }

    read(); //update value

    // if the difference is small do nothing
    if (fabs(angle_dest - angle) < precision){
        return ;
    }

    while(fabs(angle_dest - angle) >= precision){
        //angle_dest is bigger
        if (angle_dest > angle){
            increase(angle_dest);
        }
        //angle_dest is smaller
        if (angle_dest < angle){
            decrease(angle_dest);
        }
        read(); //update value
    }
    read(); //update value
}

void Angle::increase(double angle_dest){
    set_relay(arduinofd,relay_increase,COMMAND_ON);
    do{
        read(); //update value

    } while((angle_dest - angle) > precision);
    set_relay(arduinofd,relay_increase,COMMAND_OFF);
}
void Angle::decrease(double angle_dest){
    set_relay(arduinofd,relay_decrease,COMMAND_ON);
    do{
        read(); //update value
    } while((angle - angle_dest) > precision);
    set_relay(arduinofd,relay_decrease,COMMAND_OFF);
}

void Angle::read(void){
    if ( ::read(fp, &digits, sizeof(int)) == -1){
        throw std::runtime_error("unable to read from angle device");
    }
    else {
        convert();

        /* WRITE to displays*/
        displays = (int) round(angle*10); //for the first decimal part, loose the rest
        int ret_val = write(fp, &displays, sizeof(int));
        if ( ret_val == -1 || ret_val != sizeof (int)){
            throw std::runtime_error("problem writing to displays");
        }
    }
}

Beta::Beta(void){
    zero = ANGLEZERO_BETA;
    sensitivity = ANGLESENSITIVITY_BETA;
    anglemax = ANGLEMAX_BETA;
    precision = PRECISION_BETA;
    relay_increase = '2';
    relay_decrease = '3';

    QSettings settings;
    std::string filename = settings.value(SETTINGS_BETA_PATH).toString().toStdString();
    fp = open(filename.c_str(),O_RDWR);
    if ( fp == -1 ){
        throw std::runtime_error("unable to open beta device");
    }

}

void Beta::convert(void){
    angle = ((double)digits - zero) * sensitivity;
}

Alpha::Alpha(void) {
    zero = ANGLEZERO_ALPHA;
    sensitivity = ANGLESENSITIVITY_ALPHA;
    anglemax = ANGLEMAX_ALPHA;
    precision = PRECISION_ALPHA;
    relay_increase = '0';
    relay_decrease = '1';

    QSettings settings;
    std::string filename = settings.value(SETTINGS_ALPHA_PATH).toString().toStdString();
    fp = open(filename.c_str(),O_RDWR);
    if ( fp == -1 ){
        throw std::runtime_error("unable to open alpha device");
    }
}

void Alpha::convert(void){
    angle = ((double)digits - zero) * sensitivity;
    angle = atan(angle)*180/(4*atan(1)); //PI
}
