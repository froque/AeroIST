#ifndef PRESSURE_H
#define PRESSURE_H

#include "variable.h"
#include <QString>
#include <QtGui>

class PressureMeta : public VariableMeta {
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
class PressurePreferences: public VariablePreferences {
public:
    PressurePreferences();
    ~PressurePreferences();
    QWidget* get_widget();
    bool accept_config();
    bool is_configurable();
private:
    QButtonGroup *group;
};
class PressureModel : public VariableModel {
public:
    PressureModel();
    ~PressureModel();
    QWidget* view_get_widget();
    QWidget* measurement_get_widget();
    bool measurement_accept_config(VariableModel *m);
    bool measurement_is_configurable();
    void save_xml(QDomElement root);
    void load_xml(QDomElement root);

    int channel;
private:
    QButtonGroup *group;
};
class PressureHardware: public VariableHardware {
public:
    PressureHardware(VariableModel* v);
    ~PressureHardware();
    void read();
    double get_value(int n);
    void set_value(int n ,double value);
    double get_raw_value(int n);
    bool isReady(void);
    bool has_set_final();
    void set_final();
    void set_zero(QVector<double> zero);

    int channel;
private:
    int arduinofd;
    int press_raw;
    double press;
};
class PressureFactory: public QObject,public Factory {
    Q_OBJECT
    Q_INTERFACES(Factory)
public:
    VariableMeta* CreateVariableMeta();
    VariablePreferences* CreateVariableGUI();
    VariableModel* CreateVariableModel();
    VariableHardware* CreateVariableHardware(VariableModel *v);
};


#endif // PRESSURE_H
