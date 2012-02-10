#ifndef MOTOR_H
#define MOTOR_H

#include "variable.h"

class Motor : public VariableMeta {
public:
    Motor(void);
    ~Motor(void);
    void read(void);
//    void set(double);
    bool isReady(void);

    bool is_controlable() {return true;};
    int get_num() {return 1;};
    double get_value(int n);
    void set_value(int n ,double value);
    QString get_name(int n){Q_UNUSED(n); return "Motor";};
    bool has_set_final() {return true;}
    void set_final() {set_value(0,0);}
protected:

private:
    double speed_setpoint;
    double speed_actual;

    int fd;
    void convert_velocity(double percentage, unsigned char *nethigh, unsigned char *netlow);
    double convert_percentage(unsigned char nethigh, unsigned char netlow);
    void talk_to_simoreg(void);

    bool terminal37;
};


#endif // MOTOR_H
