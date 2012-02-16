#ifndef VIRTUAL_FORCE_H
#define VIRTUAL_FORCE_H

//#include "Virtual_Force_global.h"

//class VIRTUAL_FORCESHARED_EXPORT Virtual_Force {
//public:
//    Virtual_Force();
//};
#include "../variable.h"
#include "../common.h"

#include <QString>
#include <QtGui>

class Virtual_ForceMeta: public VariableMeta{
public:
    bool is_controlable();
    bool has_zero();
    int get_num();
    QString get_general_name();
    QString get_name(int n);
    QString get_units(int n);
    double get_lower_bound(int n);
    double get_upper_bound(int n);
    double get_smaller_step(int n);
    double get_default_step(int n);
    double get_default_start(int n);
};

class Virtual_ForceGUI: public VariableGUI{
public:
    Virtual_ForceGUI();
    QWidget* get_config_widget();
    bool accept_config();
    bool is_configurable();
private:
    QComboBox *combo_time;
    QLineEdit *edit;
    QComboBox *combo_matrix;
};
class Virtual_ForceModel : public VariableModel {
public:
    Virtual_ForceModel();
    int get_size();
    double get_value(int n,int row);
    QVector<double> get_vector(int n);
    void set_value(int n ,int row, double value);
    void insert_value(int n, int row, int count, double value);
    void append_value(int n, double value) ;
    void set_zero(QVector<double> zero);
    QVector<double> get_zero();
private:
    QVector<double> force[6];
    QVector<double> zero;
};

class Virtual_ForceHardware: public VariableHardware {
public:
    Virtual_ForceHardware();
    void read();
    double get_value(int n);
    void set_value(int n ,double value);
    bool isReady(void);
    bool has_set_final() ;
    void set_final() ;
    void set_zero(QVector<double> zero);
private:
    double value[6];
    QVector<double> zero;
};
class ForceFactory: public QObject,public Factory {
    Q_OBJECT
    Q_INTERFACES(Factory)
public:
    VariableMeta* CreateVariableMeta();
    VariableGUI* CreateVariableGUI();
    VariableModel* CreateVariableModel();
    VariableHardware* CreateVariableHardware() ;
};

#endif // VIRTUAL_FORCE_H
