#ifndef BOTONEIRA_H
#define BOTONEIRA_H

#define COMMAND_ON 1
#define COMMAND_OFF 0

int init_arduino(const char *port);
int set_relay(int fd, char relay , int command);

#endif // BOTONEIRA_H
