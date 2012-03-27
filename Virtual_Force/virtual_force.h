#ifndef VIRTUAL_FORCE_H
#define VIRTUAL_FORCE_H

#include "variable.h"
#include <QString>
#include <QtGui>

enum matrix_t{
    MIDDLE=0,
    FLOOR
};


class ForceMeta: public VariableMeta{
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

class ForcePreferences: public VariablePreferences{
public:
    ForcePreferences();
    QWidget* get_widget();
    bool accept_config();
    bool is_configurable();
private:
    QComboBox *combo_time;
    QLineEdit *edit;
    QComboBox *combo_matrix;
};
class ForceModel : public VariableModel {
public:
    ForceModel();
    int get_size();
    double get_value(int n,int row);
    QVector<double> get_vector(int n);
    void set_value(int n ,int row, double value);
    void insert_value(int n, int row, int count, double value);
    void append_value(int n, double value) ;
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

    int dvm_time;
    matrix_t matrix;
private:
    QVector<double> force[6],raw[6];
    QVector<double> zero;
    QComboBox *combo_time;
    QComboBox *combo_matrix;
};

class ForceHardware: public VariableHardware {
public:
    ForceHardware(VariableModel* v);
    void read();
    double get_value(int n);
    void set_value(int n ,double value);
    double get_raw_value(int n);
    bool isReady(void);
    bool has_set_final() ;
    void set_final() ;
    void set_zero(QVector<double> zero);
private:
    double value[6],raw[6];
    QVector<double> zero;
    int dvm_time;
    matrix_t matrix;
};
class ForceFactory: public QObject,public Factory {
    Q_OBJECT
    Q_INTERFACES(Factory)
public:
    VariableMeta* CreateVariableMeta();
    VariablePreferences* CreateVariableGUI();
    VariableModel* CreateVariableModel();
    VariableHardware* CreateVariableHardware(VariableModel *v);
};

#endif // VIRTUAL_FORCE_H
