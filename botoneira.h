



//extern "C" int init_arduino(char *port);
//extern "C" int set_relay(int fd, char relay , int command);

int init_arduino(const char *port);
int set_relay(int fd, char relay , int command);
