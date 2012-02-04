#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "variable.h"

class Temperature : public Variable{
public:
    Temperature();
    ~Temperature();
    void read(void);

    bool is_controlable() {return false;};
    int get_num() {return 1;};
    double get_value(int n) {Q_UNUSED(n); return temp;};
    void set_value(int n ,double value) {Q_UNUSED(n);Q_UNUSED(value)};
    QString get_name(int n) {Q_UNUSED(n);return "Temperature";}
    bool isReady(void) {return true;};

protected:
    int arduinofd;

private:
    int temp_raw;
    double temp;
};

#endif // TEMPERATURE_H
