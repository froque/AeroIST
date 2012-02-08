#ifndef VARIABLEMODEL_H
#define VARIABLEMODEL_H

#include "QVector"
#include "common.h"

class VariableModel {
public:
    virtual bool is_controlable() = 0;
    virtual int get_num() = 0;
    virtual int get_size() = 0;
    virtual double get_value(int n,int row) = 0;
    virtual QVector<double> get_vector(int n) = 0;
    virtual void set_value(int n ,int row, double value) = 0;
    virtual void insert_value(int n, int row, int count, double value) = 0;
    virtual void append_value(int n, double value) = 0;
    virtual QString get_name(int n) = 0;
    virtual QString get_units(int n) = 0;
    virtual double get_lower_bound(int n) = 0;
    virtual double get_upper_bound(int n) = 0;
    virtual double get_smaller_step(int n) = 0;
    virtual double get_default_step(int n) = 0;
    virtual double get_default_start(int n) = 0;

//    virtual bool has_zero() = 0;
//    virtual void set_zero(int n) = 0;
//    virtual double get_value_raw(int n) = 0;
//    virtual void set_value_raw(int n ,double value) = 0;
//    virtual void write_xml(???);
//    virtual void load_xml(???);
};

class ForceModel : public VariableModel {
public:
    bool is_controlable() {return false;}
    int get_num() {return 6;}
    int get_size() {return force[0].size();}
    double get_value(int n,int row) {return force[n].value(row);}
    QVector<double> get_vector(int n) {return force[n];}
    void set_value(int n ,int row, double value) { force[n].replace(row,value);}
    void insert_value(int n, int row, int count, double value) {force[n].insert(row,count,value);}
    void append_value(int n, double value) { force[n].append(value);}
    QString get_name(int n){
        switch (n){
            case 0: return "Fx"; break;
            case 1: return "Fy"; break;
            case 2: return "Fz"; break;
            case 3: return "Mx"; break;
            case 4: return "My"; break;
            case 5: return "Mz"; break;
        }
        return "";
    }
    QString get_units(int n) {
        switch (n){
        case 0:
        case 1:
        case 2:
            return "N"; break;
        case 3:
        case 4:
        case 5:
            return "N.m"; break;
        }
        return "";
    }
    double get_lower_bound(int n) {Q_UNUSED(n); return 0;}
    double get_upper_bound(int n) {Q_UNUSED(n); return 0;}
    double get_smaller_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_start(int n) {Q_UNUSED(n); return 0;}
private:
    QVector<double> force[6];
};

class AlphaModel : public VariableModel {
public:
    bool is_controlable() {return true;}
    int get_num() {return 1;}
    int get_size() {return contents.size();}
    double get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
    QVector<double> get_vector(int n) {Q_UNUSED(n); return contents;}
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
    void insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
    QString get_name(int n){Q_UNUSED(n);  return "Alpha"; }
    QString get_units(int n) { Q_UNUSED(n); return QString::fromUtf8("°"); }
    double get_lower_bound(int n) {Q_UNUSED(n); return -ANGLEMAX_ALPHA;}
    double get_upper_bound(int n) {Q_UNUSED(n); return ANGLEMAX_ALPHA;}
    double get_smaller_step(int n) {Q_UNUSED(n); return ANGLEMINSTEP_ALPHA;}
    double get_default_step(int n) {Q_UNUSED(n); return DEFAULT_ALPHA_STEP;}
    double get_default_start(int n) {Q_UNUSED(n); return DEFAULT_ALPHA_START;}
private:
    QVector<double> contents;
};
class BetaModel : public VariableModel {
public:
    bool is_controlable() {return true;}
    int get_num() {return 1;}
    int get_size() {return contents.size();}
    double get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
    QVector<double> get_vector(int n) {Q_UNUSED(n); return contents;}
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.insert(row,value);}
    void insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
    QString get_name(int n){Q_UNUSED(n);  return "Beta"; }
    QString get_units(int n) {Q_UNUSED(n); return QString::fromUtf8("°"); }
    double get_lower_bound(int n) {Q_UNUSED(n); return -ANGLEMAX_BETA;}
    double get_upper_bound(int n) {Q_UNUSED(n); return ANGLEMAX_BETA;}
    double get_smaller_step(int n) {Q_UNUSED(n); return ANGLEMINSTEP_BETA;}
    double get_default_step(int n) {Q_UNUSED(n); return DEFAULT_BETA_STEP;}
    double get_default_start(int n) {Q_UNUSED(n); return DEFAULT_BETA_START;}
private:
    QVector<double> contents;
};
class WindModel : public VariableModel {
public:
    bool is_controlable() {return false;}
    int get_num() {return 1;}
    int get_size() {return contents.size();}
    double get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
    QVector<double> get_vector(int n) {Q_UNUSED(n); return contents;}
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
    void insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
    QString get_name(int n){Q_UNUSED(n);  return "Wind"; }
    QString get_units(int n) {Q_UNUSED(n); return "mmH20"; }
    double get_lower_bound(int n) {Q_UNUSED(n); return 0;}
    double get_upper_bound(int n) {Q_UNUSED(n); return 0;}
    double get_smaller_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_start(int n) {Q_UNUSED(n); return 0;}
private:
    QVector<double> contents;
};
class MotorModel : public VariableModel {
public:
    bool is_controlable() {return true;}
    int get_num() {return 1;}
    int get_size() {return contents.size();}
    double get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
    QVector<double> get_vector(int n) {Q_UNUSED(n); return contents;}
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
    void insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
    QString get_name(int n){Q_UNUSED(n);  return "Motor"; }
    QString get_units(int n) {Q_UNUSED(n); return "%"; }
    double get_lower_bound(int n) {Q_UNUSED(n); return MOTOR_MIN;}
    double get_upper_bound(int n) {Q_UNUSED(n); return MOTOR_MAX;}
    double get_smaller_step(int n) {Q_UNUSED(n); return MOTOR_MIN_STEP;}
    double get_default_step(int n) {Q_UNUSED(n); return DEFAULT_MOTOR_STEP;}
    double get_default_start(int n) {Q_UNUSED(n); return 0;}
private:
    QVector<double> contents;
};

class TemperatureModel: public VariableModel {
public:
    bool is_controlable() {return false;}
    int get_num() {return 1;}
    int get_size() {return contents.size();}
    double get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
    QVector<double> get_vector(int n) {Q_UNUSED(n); return contents;}
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
    void insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
    QString get_name(int n){Q_UNUSED(n);  return "Temperature"; }
    QString get_units(int n) {Q_UNUSED(n); return QString::fromUtf8("°C"); }
    double get_lower_bound(int n) {Q_UNUSED(n); return 0;}
    double get_upper_bound(int n) {Q_UNUSED(n); return 0;}
    double get_smaller_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_start(int n) {Q_UNUSED(n); return 0;}
private:
    QVector<double> contents;
};
class TimeModel: public VariableModel {
public:
    bool is_controlable() {return false;}
    int get_num() {return 1;}
    int get_size() {return contents.size();}
    double get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
    QVector<double> get_vector(int n) {Q_UNUSED(n); return contents;}
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
    void insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
    QString get_name(int n){Q_UNUSED(n);  return "Time"; }
    QString get_units(int n) {Q_UNUSED(n); return "s"; }
    double get_lower_bound(int n) {Q_UNUSED(n); return 0;}
    double get_upper_bound(int n) {Q_UNUSED(n); return 0;}
    double get_smaller_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_start(int n) {Q_UNUSED(n); return 0;}
private:
    QVector<double> contents;
};

#endif // VARIABLEMODEL_H
