#include "angle.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cmath>

#include <QSettings>
#include <QDebug>

extern "C" int init_arduino(const char *port);
extern "C" int set_relay(int fd, char relay , int command);


Angle::Angle(){
    QSettings settings;
    arduinofd = init_arduino(settings.value("arduino_path").toString().toStdString().c_str());
//    arduinofd = init_arduino(ARDUINOPATH);
}

Angle::~Angle(){
    close(fp);

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
    if (fabs(angle_dest - angle) < PRECISION){
        return ;
    }

    //angle_dest is bigger
    if (angle_dest > angle){
        set_relay(arduinofd,relay_increase,1);
        do{
            read(); //update value

        } while((angle_dest - angle) > PRECISION);
        set_relay(arduinofd,relay_increase,0);
    }
    //angle_dest is smaller
    if (angle_dest < angle){
        set_relay(arduinofd,relay_decrease,1);
        do{
            read(); //update value
        } while((angle - angle_dest) > PRECISION);
        set_relay(arduinofd,relay_decrease,0);
    }
    read(); //update value
}

void Angle::read(void){
    if ( ::read(fp, &digits, sizeof(int)) == -1){
        throw;
    }
    else {
        convert();

        /* WRITE to displays*/
        displays = (int) round(angle*10); //for the first decimal part, loose the rest
        int ret_val = write(fp, &displays, sizeof(int));
        if ( ret_val == -1 || ret_val != sizeof (int)){
            throw;
        }
    }
}

Beta::Beta(void){
    zero = ANGLEZERO_BETA;
    sensitivity = ANGLESENSITIVITY_BETA;
    anglemax = ANGLEMAX_BETA;
    relay_increase = '2';
    relay_decrease = '3';

    QSettings settings;
    std::string filename = settings.value("beta_path").toString().toStdString();
    fp = open(filename.c_str(),O_RDWR);
    if ( fp == -1 ){
        throw;
    }

}

void Beta::convert(void){
    angle = ((double)digits - zero) * sensitivity;
}

Alpha::Alpha(void) {
    zero = ANGLEZERO_ALPHA;
    sensitivity = ANGLESENSITIVITY_ALPHA;
    anglemax = ANGLEMAX_ALPHA;
    relay_increase = '0';
    relay_decrease = '1';

    QSettings settings;
    std::string filename = settings.value("alpha_path").toString().toStdString();
    fp = open(filename.c_str(),O_RDWR);
    if ( fp == -1 ){
        throw;
    }
}

void Alpha::convert(void){
    angle = ((double)digits - zero) * sensitivity;
    angle = atan(angle)*180/(4*atan(1)); //PI
}
