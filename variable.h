#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>

// N variables
// maybe be contrallable
// each variable has a name and is unique
// all variables are read simultaneous
// you can set and read each one at a time
// index starts at 0

// SUBTRACTION ?

class Variable {
public:
//    virtual ~Variable() = 0;
    virtual bool is_controlable() = 0;
    virtual int get_num() = 0;
    virtual void read() = 0;
    virtual double get_value(int n) = 0;
//    virtual double get_value_raw(int n) = 0;
    virtual void set_value(int n ,double value) = 0;
    virtual QString get_name(int n) = 0;
    virtual bool isReady(void) = 0;

//    virtual bool has_zero() = 0;
//    virtual void set_zero(int n) = 0;

};

#endif // VARIABLE_H
