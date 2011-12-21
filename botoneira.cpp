#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>   /* Standard types */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <sys/time.h>
#include <stdbool.h>
#include "arduino-serial.h"
#include "botoneira.h"

#define COMMAND_ON 1
#define COMMAND_OFF 0


int init_arduino(const char *port){
    int fd;
    char b[1];
    fd = serialport_init(port,SERIALRATE);
    return fd;
}

int set_relay(int fd, char relay , int command){
    char buffer[7],buffer_read[256]="", buffer_aux[256];
    bool sucess=false;

    buffer[0]='R';
    buffer[1]=relay;
    buffer[6]='\0';
    if(command == COMMAND_ON){
        buffer[2]='o';
        buffer[3]='n';
        buffer[4]='n';
        buffer[5]='n';
    } else {
        buffer[2]='o';
        buffer[3]='f';
        buffer[4]='f';
        buffer[5]='f';
    }

    sprintf(buffer_aux,"\"%s\"",buffer);

    while (sucess == false){
        serialport_flush(fd);
        if( serialport_write(fd, buffer) == -1){
            perror("botoneira writing");
            return -1;
        }

        if( serialport_read_until(fd, buffer_read, '\n') == -1){
            perror("botoneira reading");
        }
        if (strncmp(buffer_aux,buffer_read,8)==0){
            sucess=true;
        }
    }
    return 0;
}
