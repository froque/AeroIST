#ifndef ANGLE_H
#define ANGLE_H

#include "common.h"
#include "variable.h"

class Angle :public VariableMeta{
public:
    Angle();
    ~Angle();
    void read(void);
//    void set(double);

    bool is_controlable() {return true;};
    int get_num() {return 1;};
    double get_value(int n) {Q_UNUSED(n); return angle;};
    void set_value(int n ,double value);
    virtual QString get_name(int n) = 0;
    bool isReady(void) {return true;};
    bool has_set_final() {return false;}
    void set_final() {}

protected:
    virtual void convert(void) = 0;
    double sensitivity;
    int fp;
    char relay_increase;
    char relay_decrease;
    int arduinofd;
    double precision;
    int digits;
    int zero;
    double anglemax;
    double angle;

private:
    int displays;
    void increase(double angle_dest);
    void decrease(double angle_dest);
};

class Beta: public Angle {
public:
    Beta(void);
    QString get_name(int n) {Q_UNUSED(n); return "Beta";};
private:
    void convert(void);
};

class Alpha: public Angle {
public:
    Alpha(void);
    QString get_name(int n) {Q_UNUSED(n); return "Alpha";};
private:
    void convert(void);
};

#endif // ANGLE_H
