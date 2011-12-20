#include "motor.h"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>   /* File control definitions */
#include <iostream>
#include <stdexcept>

//#include <stdio.h>  // dlete later (printf)
#include <QSettings>
#include <QDebug>
#include "common.h"
#define SIMOREG_BUFLEN 8

Motor::Motor(void){

    terminal37 = false;
    speed_setpoint = 0;
    speed_actual = 0;

    int n;
    struct termios options;
/*   fd = open(SERIALPORT, O_RDWR | O_NOCTTY | O_NDELAY);*/
//    fd = open(SERIALPORT, O_RDWR | O_NDELAY);
/*    fd = open(file, O_RDWR | O_NDELAY);*/
    QSettings settings;
    std::string filename = settings.value(SETTINGS_MOTOR_PATH).toString().toStdString();
    fd = open(filename.c_str(), O_RDWR );
    if (fd == -1){
        throw std::runtime_error("unable to open serial port");
    }

    /***** setting several options ******/
    tcflush(fd, TCIOFLUSH);
    n = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, n & ~O_NDELAY);
    /* Get the current options for the port... */
    tcgetattr(fd, &options);
    /*  Set the baud rates */
    cfsetospeed(&options, B9600);
    cfsetispeed(&options, B9600);
    /* Enable the receiver and set local mode, even parity and 8 bit word */
    options.c_cflag = (options.c_cflag & ~CSIZE) | CS8;
    options.c_cflag |= CLOCAL;
    options.c_cflag |= CREAD;
    options.c_cflag &= ~(PARENB | PARODD);
    options.c_cflag |= PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_iflag=IGNBRK;
    options.c_iflag &= ~(IXON|IXOFF|IXANY);
    options.c_lflag=0;
    options.c_oflag=0;
    options.c_cc[VTIME]=1;
//    options.c_cc[VMIN]=60;
    options.c_cc[VMIN]=0;
    /*  Set the new options for the port... */
    if (tcsetattr(fd, TCSANOW, &options)!=0){
        throw std::runtime_error("setting attributes failed");
    }

    talk_to_simoreg();
}

Motor::~Motor(void){
    close(fd);
}


void Motor::convert_velocity(double percentage, unsigned char *nethigh, unsigned char *netlow){
    if (percentage > MOTOR_FULLPERCENTAGE)
    {
        percentage = MOTOR_FULLPERCENTAGE;
    }
    if(percentage < 0)
    {
        percentage = 0;
    }
    unsigned int speed = MOTOR_FULLSPEED*percentage/MOTOR_FULLPERCENTAGE;

    *netlow  = speed%256;
    *nethigh = (speed-*netlow)/256;
}

// 0x4000 = 100%
// the drive goes from -200% to 199,9%
float Motor::convert_percentage(unsigned char nethigh, unsigned char netlow){
    int speed = nethigh * 256 + netlow;

    double percentage;
    if (speed <= MOTOR_FULLSPEED *2 ) {
        percentage =  (MOTOR_FULLPERCENTAGE * speed /MOTOR_FULLSPEED);        // 0% - 200%
    } else {
        percentage =  (MOTOR_FULLPERCENTAGE * (speed - MOTOR_FULLSPEED *4) /MOTOR_FULLSPEED); // -200%% - 0%
    }


    return percentage;
}

void Motor::talk_to_simoreg(void){

    unsigned char stx=0x02, lge=0x06, adr=0x0, net1high=0x04, net1low=0x01, net2high=0x00, net2low=0x00, bcc;
    unsigned char buffer[SIMOREG_BUFLEN];
    unsigned char buffer2[SIMOREG_BUFLEN];

    convert_velocity(speed_setpoint, &net2high, &net2low);

    bcc = stx ^ lge ^ adr ^ net1high ^ net1low ^ net2high ^ net2low;
    buffer[0] = stx;
    buffer[1] = lge;
    buffer[2] = adr;
    buffer[3] = net1high;
    buffer[4] = net1low;
    buffer[5] = net2high;
    buffer[6] = net2low;
    buffer[7] = bcc;

//    printf("SENDING:   stx:%02X lge:%02X adr:%02X net1:%02X%02X net2:%02X%02X bcc:%02X\n",stx,lge,adr,net1high,net1low,net2high,net2low,bcc);

    for(unsigned int k=0;k<sizeof(buffer);k++)
    {
//        char c= buffer[k];
//        if ( write(fd, &c, 1) <1){
        if ( write(fd, &buffer[k], 1) <1){
            throw std::runtime_error("error on writing");
        }
    }


    int bytesRead = read(fd,buffer2,SIMOREG_BUFLEN);

    if (bytesRead != SIMOREG_BUFLEN) {
        throw std::runtime_error("Error on reading from the SIMOREG serial port");
    }



    bcc=0;
    for( int k=0 ; k<bytesRead-1 ; k++) {
        bcc ^= buffer2[k];
    }

//    printf("RECEIVING: stx:%02X lge:%02X adr:%02X net1:%02X %02X net2:%02X %02X bcc:%02X\n",buffer2[0],buffer2[1],buffer2[2],buffer2[3],buffer2[4],buffer2[5],buffer2[6],buffer2[7]);

    if (bcc != buffer2[bytesRead-1]) {
        printf("bcc that should be %02X\t\t - bcc that is %02X",bcc,buffer2[bytesRead-1]);
        throw std::runtime_error("error on BCC");
    }



    terminal37 = !(buffer2[4] & 11);

    speed_actual = convert_percentage(buffer2[5], buffer2[6]);

}

void Motor::get(){
    talk_to_simoreg();

//    if (terminal37){
//        printf("terminal 37 is active\n");
//    } else {
//        printf("terminal 37 is not active\n");
//    }
}

void Motor::set(double speed){
    speed_setpoint = speed;

    talk_to_simoreg();
}

bool Motor::isReady(void){
    return terminal37;
}

// delete later
void Motor::print(void){
//    talk_to_simoreg();
    std::cout << "actual speed: " << speed_actual << std::endl;
    std::cout << "goal speed: " << speed_setpoint << std::endl;
}
