#ifndef MOTOR_H
#define MOTOR_H

#include "variable.h"
#include <QString>
#include <QtGui>

class MotorMeta : public VariableMeta {
public:
    bool is_controlable();
    bool has_zero();
    int get_num();
    QString get_general_name();
    QString get_general_name_tr();
    QString get_name(int n);
    QString get_name_tr(int n);
    QString get_units(int n);
    QString get_raw_units(int n);
    double get_lower_bound(int n);
    double get_upper_bound(int n);
    double get_smaller_step(int n);
    double get_default_step(int n);
    double get_default_start(int n);
};

class MotorPreferences : public QObject,public VariablePreferences {
    Q_OBJECT
public:
    MotorPreferences();
    ~MotorPreferences();
    QWidget* get_widget() ;
    bool accept_config() ;
    bool is_configurable();
private slots:
    void button_slot();
private:
    QLineEdit *edit_motor;
    QToolButton *button;
};


class MotorModel : public VariableModel {
public:
    MotorModel();
    ~MotorModel();
    int get_size();
    double get_value(int n,int row);
    QVector<double> get_vector(int n);
    void set_value(int n ,int row, double value);
    void insert_value(int n, int row, int count, double value);
    void append_value(int n, double value);
    double get_raw_value(int n,int row);
    void set_raw_value(int n ,int row, double value);
    void insert_raw_value(int n, int row, int count, double value);
    void append_raw_value(int n, double value);
    void set_zero(QVector<double> zero);
    QVector<double> get_zero();
    QWidget* view_get_widget();
    QWidget* measurement_get_widget();
    bool measurement_accept_config(VariableModel *m);
    bool measurement_is_configurable();
    void save_xml(QDomElement root);
    void load_xml(QDomElement root);
private:
    QVector<double> contents,raw;
};
class MotorHardware: public VariableHardware {
public:
    MotorHardware ();
    ~MotorHardware ();
    void read();
    double get_value(int n);
    void set_value(int n ,double value);
    double get_raw_value(int n);
    bool isReady(void);
    bool has_set_final();
    void set_final();
    void set_zero(QVector<double> zero);
private:
    double speed_setpoint;
    double speed_actual;

    int fd;
    void convert_velocity(double percentage, unsigned char *nethigh, unsigned char *netlow);
    double convert_percentage(unsigned char nethigh, unsigned char netlow);
    void talk_to_simoreg(void);

    bool terminal37;
};
class MotorFactory: public QObject,public Factory {
    Q_OBJECT
    Q_INTERFACES(Factory)
public:
    VariableMeta* CreateVariableMeta();
    VariablePreferences* CreateVariableGUI();
    VariableModel* CreateVariableModel();
    VariableHardware* CreateVariableHardware(VariableModel *v);
};

#endif // MOTOR_H
