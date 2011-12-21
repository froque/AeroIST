#ifndef MOTOR_H
#define MOTOR_H

class Motor {
public:
    Motor(void);
    ~Motor(void);
    void read(void);
    void set(double);
    bool isReady(void);
    double speed_setpoint;
    double speed_actual;

protected:

private:
    int fd;
    void convert_velocity(double percentage, unsigned char *nethigh, unsigned char *netlow);
    double convert_percentage(unsigned char nethigh, unsigned char netlow);
    void talk_to_simoreg(void);

    bool terminal37;
};


#endif // MOTOR_H
