#ifndef VIRTUAL_MOTOR_H
#define VIRTUAL_MOTOR_H

#include "../variable.h"
#include "../common.h"
#include "../helper.h"
#include <QString>
#include <QtGui>

class Virtual_MotorMeta : public VariableMeta {
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

class Virtual_MotorGUI : public QObject,public VariableGUI {
    Q_OBJECT
public:
    Virtual_MotorGUI();
    QWidget* get_config_widget() ;

    bool accept_config() ;

    bool is_configurable();

private slots:
    void button_slot();
private:
    QLineEdit *edit_motor;
    QToolButton *button;
};


class Virtual_MotorModel : public VariableModel {
public:
    Virtual_MotorModel();
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
};
class Virtual_MotorHardware: public VariableHardware {
public:
    Virtual_MotorHardware ();
    void read();
    double get_value(int n);
    void set_value(int n ,double value);
    QString get_name(int n);
    bool isReady(void);
    bool has_set_final();
    void set_final();
    void set_zero(QVector<double> zero);
private:
    double value;
    bool control_set;
};
class MotorFactory: public QObject,public Factory {
    Q_OBJECT
    Q_INTERFACES(Factory)
public:
    VariableMeta* CreateVariableMeta();
    VariableGUI* CreateVariableGUI();
    VariableModel* CreateVariableModel();
    VariableHardware* CreateVariableHardware();
};


#endif // VIRTUAL_MOTOR_H
