#ifndef VARIABLEMODEL_H
#define VARIABLEMODEL_H

#include "QVector"

class VariableModel {
public:
    virtual bool is_controlable() = 0;
    virtual int get_num() = 0;
    virtual int get_size() = 0;
    virtual double get_value(int n,int row) = 0;
    virtual QVector<double> get_vector(int n) = 0;
    virtual void set_value(int n ,int row, double value) = 0;
    virtual void append_value(int n, double value) = 0;
    virtual QString get_name(int n) = 0;

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
    void set_value(int n ,int row, double value) { force[n].insert(row,value);}
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
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.insert(row,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
    QString get_name(int n){Q_UNUSED(n);  return "Alpha"; }
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
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
    QString get_name(int n){Q_UNUSED(n);  return "Beta"; }
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
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.insert(row,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
    QString get_name(int n){Q_UNUSED(n);  return "Wind"; }
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
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.insert(row,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
    QString get_name(int n){Q_UNUSED(n);  return "Motor"; }
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
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.insert(row,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
    QString get_name(int n){Q_UNUSED(n);  return "Temperature"; }
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
    void set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.insert(row,value);}
    void append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
    QString get_name(int n){Q_UNUSED(n);  return "Time"; }
private:
    QVector<double> contents;
};


#endif // VARIABLEMODEL_H
