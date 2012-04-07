#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>
#include <QWidget>
#include <QDomElement>

// N variables
// maybe be contrallable
// each variable has a name and is unique
// all variables are read simultaneous
// you can set and read each one at a time
// index starts at 0
// a controlable variable maybe set to a safe value in the final

// raw ?

class VariableMeta {
public:
    virtual ~VariableMeta() {};
    virtual bool is_controlable() = 0;
    virtual bool has_zero() = 0;
    virtual int get_num() = 0;
    virtual QString get_general_name() = 0;
    // should be translatable
    virtual QString get_general_name_tr() = 0;
    virtual QString get_name(int n) = 0;
    // should be translatable
    virtual QString get_name_tr(int n) = 0;
    // should be translatable
    virtual QString get_units(int n) = 0;
    virtual double get_lower_bound(int n) = 0;
    virtual double get_upper_bound(int n) = 0;
    virtual double get_smaller_step(int n) = 0;
    virtual double get_default_step(int n) = 0;
    virtual double get_default_start(int n) = 0;
};

class VariablePreferences {
public:
    virtual ~VariablePreferences(){};
    virtual QWidget* get_widget() = 0;
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
    virtual double get_raw_value(int n,int row) = 0;
    virtual void set_raw_value(int n ,int row, double value) = 0;
    virtual void insert_raw_value(int n, int row, int count, double value) = 0;
    virtual void append_raw_value(int n, double value) = 0;
    virtual void set_zero(QVector<double> zero) = 0;
    virtual QVector<double> get_zero() = 0;

    virtual QWidget* view_get_widget() = 0;  // details
    virtual QWidget* measurement_get_widget() = 0; // preferences
    virtual bool measurement_accept_config(VariableModel *m) = 0;
    virtual bool measurement_is_configurable() = 0;
    virtual void save_xml(QDomElement root) = 0;
    virtual void load_xml(QDomElement root) = 0;
    VariableMeta *meta;
    QVector<double> start;
};

class VariableHardware{
public:
    virtual ~VariableHardware() {}
    virtual void read() = 0;
    virtual double get_value(int n) = 0;
    virtual double get_raw_value(int n) = 0;
    virtual void set_value(int n ,double value) = 0;
    virtual bool isReady(void) = 0;
    virtual bool has_set_final() = 0;
    virtual void set_final() = 0;
    VariableMeta *meta;
    virtual void set_zero(QVector<double> zero) = 0;
    QVector<double> start;
};

class Factory {
public:
    virtual ~Factory() {}
    virtual VariableMeta* CreateVariableMeta() = 0;
    virtual VariablePreferences* CreateVariableGUI() = 0;
    virtual VariableModel* CreateVariableModel() = 0;
    virtual VariableHardware* CreateVariableHardware(VariableModel* v) = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(Factory, "pt.ist.utl.aeroist.factory/1.4")
QT_END_NAMESPACE

#endif // VARIABLE_H
