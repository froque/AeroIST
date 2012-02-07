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

    virtual bool has_set_final() = 0;
    virtual void set_final() = 0;

//    virtual bool has_zero() = 0;
//    virtual void set_zero(int n) = 0;

//    virtual bool has_control() = 0;
//    virtual void set_control(bool control) = 0;

//    int get_index(QString name){
//        for (int k = 0 ; k< get_num(); k++){
//            if (name == get_name(k)){
//                return k;
//            }
//        }
//        return -1;
//    }
};

class Virtual_Alpha: public Variable {
public:
    Virtual_Alpha(){ value=0; control_set=false;}
    bool is_controlable() {return true;}
    int get_num() {return 1;}
    void read() {if(control_set==false){ value = -10.0 * qrand() / RAND_MAX;}}
    double get_value(int n) {Q_UNUSED(n); return value;}
    void set_value(int n ,double value) {Q_UNUSED(n);  control_set=true; this->value = value;}
    QString get_name(int n) {Q_UNUSED(n); return "Alpha";}

    bool isReady(void) {return true;}
    bool has_set_final() {return is_controlable() && false;}
    void set_final() {}
private:
    double value;
    bool control_set;
};
class Virtual_Beta: public Variable {
public:
    Virtual_Beta () { value=0; control_set=false;}
    bool is_controlable() {return true;}
    int get_num() {return 1;}
    void read() {if(control_set==false){ value = -10.0 * qrand() / RAND_MAX;}}
    double get_value(int n) {Q_UNUSED(n); return value;}
    void set_value(int n ,double value) {Q_UNUSED(n);  control_set=true; this->value = value;}
    QString get_name(int n) {Q_UNUSED(n); return "Beta";}
    bool isReady(void) {return true;}
    bool has_set_final() {return is_controlable() && false;}
    void set_final() {}
private:
    double value;
    bool control_set;
};
class Virtual_Motor: public Variable {
public:
    Virtual_Motor () { value=0; control_set=false;}
    bool is_controlable() {return true;}
    int get_num() {return 1;}
    void read() {if(control_set==false){ value = -10.0 * qrand() / RAND_MAX;}}
    double get_value(int n) {Q_UNUSED(n); return value;}
    void set_value(int n ,double value) {Q_UNUSED(n);  control_set=true; this->value = value;}
    QString get_name(int n) {Q_UNUSED(n); return "Motor";}
    bool isReady(void) {return true;}
    bool has_set_final() {return is_controlable() && true;}
    void set_final() {set_value(0,0);}
private:
    double value;
    bool control_set;
};
class Virtual_Wind: public Variable {
public:
    bool is_controlable() {return false;}
    int get_num() {return 1;}
    void read() {value = -10.0 * qrand() / RAND_MAX;}
    double get_value(int n) {Q_UNUSED(n); return value;}
    void set_value(int n ,double value) {Q_UNUSED(n); Q_UNUSED(value);  }
    QString get_name(int n) {Q_UNUSED(n); return "Wind";}
    bool isReady(void) {return true;}
    bool has_set_final() {return is_controlable() && false;}
    void set_final() {}
private:
    double value;
};
class Virtual_Temperature: public Variable {
public:
    bool is_controlable() {return false;}
    int get_num() {return 1;}
    void read() {value = -10.0 * qrand() / RAND_MAX;}
    double get_value(int n) {Q_UNUSED(n); return value;}
    void set_value(int n ,double value) {Q_UNUSED(n); Q_UNUSED(value);  }
    QString get_name(int n) {Q_UNUSED(n); return "Temperature";}
    bool isReady(void) {return true;}
    bool has_set_final() {return is_controlable() && false;}
    void set_final() {}
private:
    double value;
};
class Virtual_Force: public Variable {
public:
    bool is_controlable() {return false;}
    int get_num() {return 6;}
    void read() {Helper::msleep(500); for (int k=0; k<6; k++) {value[k] = -10.0 * qrand() / RAND_MAX;}}
    double get_value(int n) { return value[n];}
    void set_value(int n ,double value) {Q_UNUSED(n); Q_UNUSED(value);  }
    QString get_name(int n) {
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
    bool isReady(void) {return true;}
    bool has_set_final() {return is_controlable() && false;}
    void set_final() {}
private:
    double value[6];
};

#endif // VARIABLE_H
