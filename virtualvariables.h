#ifndef VIRTUALVARIABLES_H
#define VIRTUALVARIABLES_H

#include "variable.h"
#include <QVector>
#include <QDebug>

class Virtual_Force: public VariableMeta{
public:
//    ~Virtual_Force(){};
    bool is_controlable() {return false;}
    int get_num() {return 6;}
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
};

class Virtual_ForceModel : public VariableModel {
public:
    Virtual_ForceModel(){meta = new Virtual_Force;}
    int get_size() {return force[0].size();}
    double get_value(int n,int row) {return force[n].value(row);}
    QVector<double> get_vector(int n) {return force[n];}
    void set_value(int n ,int row, double value) { force[n].replace(row,value);}
    void insert_value(int n, int row, int count, double value) {force[n].insert(row,count,value);}
    void append_value(int n, double value) { force[n].append(value);}
private:
    QVector<double> force[6];
};

class Virtual_ForceHardware: public VariableHardware {
public:
    Virtual_ForceHardware(){meta = new Virtual_Force;}
    void read() {Helper::msleep(500); for (int k=0; k<6; k++) {value[k] = -10.0 * qrand() / RAND_MAX;}}
    double get_value(int n) { return value[n];}
    void set_value(int n ,double value) {Q_UNUSED(n); Q_UNUSED(value);  }
    bool isReady(void) {return true;}
    bool has_set_final() {return meta->is_controlable() && false;}
//    bool has_set_final() {return false;}
    void set_final() {}
private:
    double value[6];
};


class Virtual_AlphaMeta : public VariableMeta {
public:
    bool is_controlable() {return true;}
    int get_num() {return 1;}
    QString get_name(int n){Q_UNUSED(n);  return "Alpha"; }
    QString get_units(int n) { Q_UNUSED(n); return QString::fromUtf8("°"); }
    double get_lower_bound(int n) {Q_UNUSED(n); return -ANGLEMAX_ALPHA;}
    double get_upper_bound(int n) {Q_UNUSED(n); return ANGLEMAX_ALPHA;}
    double get_smaller_step(int n) {Q_UNUSED(n); return ANGLEMINSTEP_ALPHA;}
    double get_default_step(int n) {Q_UNUSED(n); return DEFAULT_ALPHA_STEP;}
    double get_default_start(int n) {Q_UNUSED(n); return DEFAULT_ALPHA_START;}
};

class Virtual_AlphaModel : public VariableModel {
public:
    Virtual_AlphaModel(){meta = new Virtual_AlphaMeta;}
    int get_size() {return contents.size();}
    double get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
    QVector<double> get_vector(int n) {Q_UNUSED(n); return contents;}
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
    void insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
private:
    QVector<double> contents;
};
class Virtual_AlphaHardware: public VariableHardware {
public:
    Virtual_AlphaHardware(){meta = new Virtual_AlphaMeta; value=0; control_set=false;}
    void read() {if(control_set==false){ value = -10.0 * qrand() / RAND_MAX;}}
    double get_value(int n) {Q_UNUSED(n); return value;}
    void set_value(int n ,double value) {Q_UNUSED(n);  control_set=true; this->value = value;}
    bool isReady(void) {return true;}
    bool has_set_final() {return meta->is_controlable() && false;}
    void set_final() {}
private:
    double value;
    bool control_set;
};



class Virtual_BetaMeta: public VariableMeta{
public:
    bool is_controlable() {return true;}
    int get_num() {return 1;}
    QString get_name(int n){Q_UNUSED(n);  return "Beta"; }
    QString get_units(int n) {Q_UNUSED(n); return QString::fromUtf8("°"); }
    double get_lower_bound(int n) {Q_UNUSED(n); return -ANGLEMAX_BETA;}
    double get_upper_bound(int n) {Q_UNUSED(n); return ANGLEMAX_BETA;}
    double get_smaller_step(int n) {Q_UNUSED(n); return ANGLEMINSTEP_BETA;}
    double get_default_step(int n) {Q_UNUSED(n); return DEFAULT_BETA_STEP;}
    double get_default_start(int n) {Q_UNUSED(n); return DEFAULT_BETA_START;}
};
class Virtual_BetaModel : public VariableModel {
public:
    Virtual_BetaModel(){meta = new Virtual_BetaMeta;}
    int get_size() {return contents.size();}
    double get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
    QVector<double> get_vector(int n) {Q_UNUSED(n); return contents;}
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.insert(row,value);}
    void insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
private:
    QVector<double> contents;
};
class Virtual_BetaHardware: public VariableHardware {
public:
    Virtual_BetaHardware () {meta = new Virtual_BetaMeta; value=0; control_set=false;}
    void read() {if(control_set==false){ value = -10.0 * qrand() / RAND_MAX;}}
    double get_value(int n) {Q_UNUSED(n); return value;}
    void set_value(int n ,double value) {Q_UNUSED(n);  control_set=true; this->value = value;}
    bool isReady(void) {return true;}
    bool has_set_final() {return meta->is_controlable() && false;}
    void set_final() {}
private:
    double value;
    bool control_set;
};


class Virtual_WindMeta : public VariableMeta {
public:
    bool is_controlable() {return false;}
    int get_num() {return 1;}
    QString get_name(int n){Q_UNUSED(n);  return "Wind"; }
    QString get_units(int n) {Q_UNUSED(n); return "mmH20"; }
    double get_lower_bound(int n) {Q_UNUSED(n); return 0;}
    double get_upper_bound(int n) {Q_UNUSED(n); return 0;}
    double get_smaller_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_start(int n) {Q_UNUSED(n); return 0;}
};
class Virtual_WindModel : public VariableModel {
public:
    Virtual_WindModel(){meta = new Virtual_WindMeta;}
    int get_size() {return contents.size();}
    double get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
    QVector<double> get_vector(int n) {Q_UNUSED(n); return contents;}
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
    void insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
private:
    QVector<double> contents;
};
class Virtual_WindHardware: public VariableHardware {
public:
    Virtual_WindHardware() {meta = new Virtual_WindMeta;}
    void read() { value = -10.0 * qrand() / RAND_MAX;}
    double get_value(int n) {Q_UNUSED(n); return value;}
    void set_value(int n ,double value) {Q_UNUSED(n); Q_UNUSED(value);  }
    bool isReady(void) {return true;}
    bool has_set_final() {return meta->is_controlable() && false;}
    void set_final() {}
private:
    double value;
};
class Virtual_MotorMeta : public VariableMeta {
public:
    bool is_controlable() {return true;}
    int get_num() {return 1;}
    QString get_name(int n){Q_UNUSED(n);  return "Motor"; }
    QString get_units(int n) {Q_UNUSED(n); return "%"; }
    double get_lower_bound(int n) {Q_UNUSED(n); return MOTOR_MIN;}
    double get_upper_bound(int n) {Q_UNUSED(n); return MOTOR_MAX;}
    double get_smaller_step(int n) {Q_UNUSED(n); return MOTOR_MIN_STEP;}
    double get_default_step(int n) {Q_UNUSED(n); return DEFAULT_MOTOR_STEP;}
    double get_default_start(int n) {Q_UNUSED(n); return 0;}
};
class Virtual_MotorModel : public VariableModel {
public:
    Virtual_MotorModel(){meta = new Virtual_MotorMeta;}
    int get_size() {return contents.size();}
    double get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
    QVector<double> get_vector(int n) {Q_UNUSED(n); return contents;}
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
    void insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
private:
    QVector<double> contents;
};
class Virtual_MotorHardware: public VariableHardware {
public:
    Virtual_MotorHardware () {meta = new Virtual_MotorMeta; value=0; control_set=false;}
    void read() {if(control_set==false){ value = -10.0 * qrand() / RAND_MAX;}}
    double get_value(int n) {Q_UNUSED(n); return value;}
    void set_value(int n ,double value) {Q_UNUSED(n);  control_set=true; this->value = value;}
    QString get_name(int n) {Q_UNUSED(n); return "Motor";}
    bool isReady(void) {return true;}
    bool has_set_final() {return meta->is_controlable() && true;}
    void set_final() {set_value(0,0);}
private:
    double value;
    bool control_set;
};


class Virtual_TemperatureMeta: public VariableMeta {
public:
    bool is_controlable() {return false;}
    int get_num() {return 1;}
    QString get_name(int n){Q_UNUSED(n);  return "Temperature"; }
    QString get_units(int n) {Q_UNUSED(n); return QString::fromUtf8("°C"); }
    double get_lower_bound(int n) {Q_UNUSED(n); return 0;}
    double get_upper_bound(int n) {Q_UNUSED(n); return 0;}
    double get_smaller_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_start(int n) {Q_UNUSED(n); return 0;}
};
class Virtual_TemperatureModel: public VariableModel {
public:
    Virtual_TemperatureModel(){meta = new Virtual_TemperatureMeta;}
    int get_size() {return contents.size();}
    double get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
    QVector<double> get_vector(int n) {Q_UNUSED(n); return contents;}
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
    void insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
private:
    QVector<double> contents;
};
class Virtual_TemperatureHardware: public VariableHardware {
public:
    Virtual_TemperatureHardware() {meta = new Virtual_TemperatureMeta;}
    void read() { value = -10.0 * qrand() / RAND_MAX;}
    double get_value(int n) {Q_UNUSED(n); return value;}
    void set_value(int n ,double value) {Q_UNUSED(n); Q_UNUSED(value);  }
    bool isReady(void) {return true;}
    bool has_set_final() {return meta->is_controlable() && false;}
    void set_final() {}
private:
    double value;
};


class Virtual_TimeMeta: public VariableMeta {
public:
    bool is_controlable() {return false;}
    int get_num() {return 1;}
    QString get_name(int n){Q_UNUSED(n);  return "Time"; }
    QString get_units(int n) {Q_UNUSED(n); return "s"; }
    double get_lower_bound(int n) {Q_UNUSED(n); return 0;}
    double get_upper_bound(int n) {Q_UNUSED(n); return 0;}
    double get_smaller_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_start(int n) {Q_UNUSED(n); return 0;}
};
class Virtual_TimeModel: public VariableModel {
public:
    Virtual_TimeModel(){meta = new Virtual_TimeMeta;}
    int get_size() {return contents.size();}
    double get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
    QVector<double> get_vector(int n) {Q_UNUSED(n); return contents;}
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
    void insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
private:
    QVector<double> contents;
};



#endif // VIRTUALVARIABLES_H
