#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>
#include <cstdlib> // for RAND_MAX
#include "helper.h" //for mleep

// N variables
// maybe be contrallable
// each variable has a name and is unique
// all variables are read simultaneous
// you can set and read each one at a time
// index starts at 0
// a controlable variable maybe set to a safe value in the final

// SUBTRACTION ?
// raw ?
//    virtual bool has_zero() = 0; ??
//    virtual void set_zero(int n) = 0; ??

class VariableMeta {
public:
    virtual ~VariableMeta() {};
    virtual bool is_controlable() = 0;
    virtual int get_num() = 0;
    virtual QString get_general_name() = 0;
    virtual QString get_name(int n) = 0;
    virtual QString get_units(int n) = 0;
    virtual double get_lower_bound(int n) = 0;
    virtual double get_upper_bound(int n) = 0;
    virtual double get_smaller_step(int n) = 0;
    virtual double get_default_step(int n) = 0;
    virtual double get_default_start(int n) = 0;
};

class VariableGUI {
public:
    virtual QWidget* get_config_widget() = 0;
    virtual bool accept_config() = 0;
    virtual bool is_configurable() = 0;
    VariableMeta *meta;
};

class VariableModel{
public:
    virtual ~VariableModel() {}
    virtual int get_size() = 0;
    virtual double get_value(int n,int row) = 0;
    virtual QVector<double> get_vector(int n) = 0;
    virtual void set_value(int n ,int row, double value) = 0;
    virtual void insert_value(int n, int row, int count, double value) = 0;
    virtual void append_value(int n, double value) = 0;
    VariableMeta *meta;
};

class VariableHardware{
public:
    virtual ~VariableHardware() {}
    virtual void read() = 0;
    virtual double get_value(int n) = 0;
    virtual void set_value(int n ,double value) = 0;
    virtual bool isReady(void) = 0;
    virtual bool has_set_final() = 0;
    virtual void set_final() = 0;
    VariableMeta *meta;
};


#endif // VARIABLE_H
