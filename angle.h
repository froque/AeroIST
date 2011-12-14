#ifndef ANGLE_H
#define ANGLE_H

#include "common.h"

class Angle {
public:
    Angle();
    ~Angle();
    void read(void);
    void set(double);
    double angle;
    int digits;
    int zero;
    double anglemax;

protected:
    virtual void convert(void) =0;
    double sensitivity;
    int fp;
    char relay_increase;
    char relay_decrease;
    int arduinofd;
    double precision;
private:
    int displays;
    void increase(double angle_dest);
    void decrease(double angle_dest);
};

class Beta: public Angle {
public:
    Beta(void);
private:
    void convert(void);
};

class Alpha: public Angle {
public:
    Alpha(void);
private:
    void convert(void);
};

#endif // ANGLE_H
