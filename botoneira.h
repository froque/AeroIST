#ifndef BOTONEIRA_H
#define BOTONEIRA_H


int init_arduino(const char *port);
int set_relay(int fd, char relay , int command);

#endif // BOTONEIRA_H
