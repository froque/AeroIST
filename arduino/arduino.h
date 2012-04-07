#ifndef ARDUINO_H
#define ARDUINO_H

#include "variable.h"
#include <QString>
#include <QtGui>


class ArduinoMeta : public VariableMeta {
public:
    bool is_controlable();
    bool has_zero();
    int get_num();
    QString get_general_name();
    QString get_general_name_tr();
    QString get_name(int n);
    QString get_name_tr(int n);
    QString get_units(int n);
    double get_lower_bound(int n);
    double get_upper_bound(int n);
    double get_smaller_step(int n);
    double get_default_step(int n);
    double get_default_start(int n);
};

class ArduinoPreferences: public VariablePreferences {
public:
    ArduinoPreferences();
    QWidget* get_widget();
    bool accept_config();
    bool is_configurable();
private:
    QLineEdit *edit_arduino;
};


class ArduinoFactory: public QObject,public Factory {
    Q_OBJECT
    Q_INTERFACES(Factory)
public:
    VariableMeta* CreateVariableMeta() ;
    VariablePreferences* CreateVariableGUI() ;
    VariableModel* CreateVariableModel() ;
    VariableHardware* CreateVariableHardware(VariableModel *v);
};

#endif // ARDUINO_H
