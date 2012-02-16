#ifndef VIRTUAL_TIME_H
#define VIRTUAL_TIME_H

#include "../variable.h"
#include "../common.h"
#include "../helper.h"
#include <QString>
#include <QtGui>

class Virtual_TimeMeta: public VariableMeta {
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
class Virtual_TimeGUI: public VariableGUI {
public:
    Virtual_TimeGUI();
    QWidget* get_config_widget();

    bool accept_config();
    bool is_configurable();
private:
    QLineEdit *edit_beta;
};
class Virtual_TimeModel: public VariableModel {
public:
    Virtual_TimeModel();
    int get_size();
    double get_value(int n,int row);
    QVector<double> get_vector(int n);
    void set_value(int n ,int row, double value);
    void insert_value(int n, int row, int count, double value);
    void append_value(int n, double value);
    void set_zero(QVector<double> zero);
    QVector<double> get_zero();
private:
    QVector<double> contents;
    QVector<double> zero;
};
class TimeFactory: public QObject,public Factory {
    Q_OBJECT
    Q_INTERFACES(Factory)
public:
    VariableMeta* CreateVariableMeta();
    VariableGUI* CreateVariableGUI();
    VariableModel* CreateVariableModel();
//    VariableHardware* CreateVariableHardware();
    VariableHardware* CreateVariableHardware();
};


#endif // VIRTUAL_TIME_H
