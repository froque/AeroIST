#ifndef ARDUINO_H
#define ARDUINO_H

#define SERIALRATE 9600

int serialport_init(const char* serialport, int baud);
int serialport_writebyte(int fd, uint8_t b);
int serialport_write(int fd, const char* str);
int serialport_read_until(int fd, char* buf, char until);
void serialport_flush(int fd);

#endif // ARDUINO_H
