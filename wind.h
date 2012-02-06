#ifndef WIND_H
#define WIND_H

#include "variable.h"

class Wind: public Variable
{
public:
    Wind();
    ~Wind();
    void read(void);

    bool is_controlable() {return false;};
    int get_num() {return 1;};
    double get_value(int n) {Q_UNUSED(n); return wind;};
    void set_value(int n ,double value) {Q_UNUSED(n);Q_UNUSED(value)};
    QString get_name(int n) {Q_UNUSED(n);return "Wind";}
    bool isReady(void) {return true;};
    bool has_set_final() {return false;}
    void set_final() {}
protected:
    int arduinofd;

private:
    int wind_raw;
    double wind;
};

#endif // WIND_H
