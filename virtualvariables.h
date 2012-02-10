#ifndef VIRTUALVARIABLES_H
#define VIRTUALVARIABLES_H

#include "variable.h"
#include <QVector>
#include <QDebug>
#include <QtGui>
#include <QFileDialog>

#include "helper.h" //for mleep
#include "common.h"

class Virtual_ForceMeta: public VariableMeta{
public:
//    ~Virtual_Force(){};
    bool is_controlable() {return false;}
    int get_num() {return 6;}
    QString get_general_name(){ return "Forces";}
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

class Virtual_ForceGUI: public VariableGUI{
public:
    Virtual_ForceGUI(){meta = new Virtual_ForceMeta;}
    QWidget* get_config_widget() {
        QWidget *widget = new QWidget;
        QGridLayout *layout = new QGridLayout;
        QSettings settings;
        layout->addWidget(new QLabel(QObject::tr("Multimeter")),0,0);
        edit = new QLineEdit(settings.value(SETTINGS_MULTIMETER_PATH, SETTINGS_MULTIMETER_PATH_DEFAULT).toString());
        layout->addWidget(edit,0,1);
        layout->addWidget(new QLabel(QObject::tr("Default Multimeter time")),1,0);
        combo_time = new QComboBox;
        combo_time->addItem(QObject::tr("50 ms"),    1);
        combo_time->addItem(QObject::tr("100 ms"),   2);
        combo_time->addItem(QObject::tr("500 ms"),   3);
        combo_time->addItem(QObject::tr("1 s"),      4);
        combo_time->addItem(QObject::tr("5 s"),      5);
        combo_time->addItem(QObject::tr("10 s"),     6);
        int index = combo_time->findData(settings.value(SETTINGS_DEFAULT_DVM_TIME).toInt());
        combo_time->setCurrentIndex(index);
        layout->addWidget(combo_time,1,1);
        layout->addWidget(new QLabel(QObject::tr("Default Matrix")),2,0);
        combo_matrix =  new QComboBox;
        combo_matrix->addItem(QObject::tr("middle"), MIDDLE);
        combo_matrix->addItem(QObject::tr("floor"),  FLOOR);
        index = combo_matrix->findData(settings.value(SETTINGS_DEFAULT_MATRIX).toInt());
        combo_matrix->setCurrentIndex(index);
        layout->addWidget(combo_matrix,2,1);
        widget->setLayout(layout);
        return widget;
    }
    bool accept_config() {
        QSettings settings;
        int index = combo_time->currentIndex();
        settings.setValue(SETTINGS_DEFAULT_DVM_TIME, combo_time->itemData(index).toInt());
        settings.setValue(SETTINGS_MULTIMETER_PATH, edit->text());
        index = combo_matrix->currentIndex();
        settings.setValue(SETTINGS_DEFAULT_MATRIX, combo_matrix->itemData(index).toInt());
        return true;
    }
    bool is_configurable() {return true;}
private:
    QComboBox *combo_time;
    QLineEdit *edit;
    QComboBox *combo_matrix;
};


class Virtual_ForceModel : public VariableModel {
public:
    Virtual_ForceModel(){meta = new Virtual_ForceMeta;}
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
    Virtual_ForceHardware(){meta = new Virtual_ForceMeta;}
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
    QString get_general_name(){ return "Alpha";}
    QString get_name(int n){Q_UNUSED(n);  return "Alpha"; }
    QString get_units(int n) { Q_UNUSED(n); return QString::fromUtf8("°"); }
    double get_lower_bound(int n) {Q_UNUSED(n); return -ANGLEMAX_ALPHA;}
    double get_upper_bound(int n) {Q_UNUSED(n); return ANGLEMAX_ALPHA;}
    double get_smaller_step(int n) {Q_UNUSED(n); return ANGLEMINSTEP_ALPHA;}
    double get_default_step(int n) {Q_UNUSED(n); return DEFAULT_ALPHA_STEP;}
    double get_default_start(int n) {Q_UNUSED(n); return DEFAULT_ALPHA_START;}
};

class Virtual_AlphaGUI: public VariableGUI {
public:
    Virtual_AlphaGUI() {meta = new Virtual_AlphaMeta();}
    QWidget* get_config_widget() {
        QWidget *widget = new QWidget;
        QGridLayout *layout = new QGridLayout;
        QSettings settings;
        layout->addWidget(new QLabel(QObject::tr("Alpha device path")),0,0);
        edit_alpha = new QLineEdit;
        edit_alpha->setText(settings.value(SETTINGS_ALPHA_PATH).toString());
        layout->addWidget(edit_alpha,0,1);
        widget->setLayout(layout);
        return widget;
    }

    bool accept_config() {
        QSettings settings;
        settings.setValue(SETTINGS_ALPHA_PATH, edit_alpha->text());
        return true;
    }
    bool is_configurable() {return true;}
private:
    QLineEdit *edit_alpha;
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
    QString get_general_name(){ return "Beta";}
    QString get_name(int n){Q_UNUSED(n);  return "Beta"; }
    QString get_units(int n) {Q_UNUSED(n); return QString::fromUtf8("°"); }
    double get_lower_bound(int n) {Q_UNUSED(n); return -ANGLEMAX_BETA;}
    double get_upper_bound(int n) {Q_UNUSED(n); return ANGLEMAX_BETA;}
    double get_smaller_step(int n) {Q_UNUSED(n); return ANGLEMINSTEP_BETA;}
    double get_default_step(int n) {Q_UNUSED(n); return DEFAULT_BETA_STEP;}
    double get_default_start(int n) {Q_UNUSED(n); return DEFAULT_BETA_START;}
};
class Virtual_BetaGUI: public VariableGUI {
public:
    Virtual_BetaGUI() {meta = new Virtual_BetaMeta();}
    QWidget* get_config_widget() {
        QWidget *widget = new QWidget;
        QGridLayout *layout = new QGridLayout;
        QSettings settings;
        layout->addWidget(new QLabel(QObject::tr("Beta device path")),0,0);
        edit_beta = new QLineEdit;
        edit_beta->setText(settings.value(SETTINGS_BETA_PATH).toString());
        layout->addWidget(edit_beta,0,1);
        widget->setLayout(layout);
        return widget;
    }

    bool accept_config() {
        QSettings settings;
        settings.setValue(SETTINGS_BETA_PATH, edit_beta->text());
        return true;
    }
    bool is_configurable() {return true;}
private:
    QLineEdit *edit_beta;
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
    QString get_general_name(){ return "Wind";}
    QString get_name(int n){Q_UNUSED(n);  return "Wind"; }
    QString get_units(int n) {Q_UNUSED(n); return "mmH20"; }
    double get_lower_bound(int n) {Q_UNUSED(n); return 0;}
    double get_upper_bound(int n) {Q_UNUSED(n); return 0;}
    double get_smaller_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_start(int n) {Q_UNUSED(n); return 0;}
};
class Virtual_WindGUI: public VariableGUI {
public:
    Virtual_WindGUI() {meta = new Virtual_WindMeta();}
    QWidget* get_config_widget() {return NULL;}
    bool accept_config() {return true;}
    bool is_configurable() {return true;}
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
    QString get_general_name(){ return "Motor";}
    QString get_name(int n){Q_UNUSED(n);  return "Motor"; }
    QString get_units(int n) {Q_UNUSED(n); return "%"; }
    double get_lower_bound(int n) {Q_UNUSED(n); return MOTOR_MIN;}
    double get_upper_bound(int n) {Q_UNUSED(n); return MOTOR_MAX;}
    double get_smaller_step(int n) {Q_UNUSED(n); return MOTOR_MIN_STEP;}
    double get_default_step(int n) {Q_UNUSED(n); return DEFAULT_MOTOR_STEP;}
    double get_default_start(int n) {Q_UNUSED(n); return 0;}
};

class Virtual_MotorGUI : public QObject,public VariableGUI {
    Q_OBJECT
public:
    Virtual_MotorGUI() {meta = new Virtual_MotorMeta();}
    QWidget* get_config_widget() {
        QWidget *widget = new QWidget;
        QGridLayout *layout = new QGridLayout;
        QSettings settings;
        layout->addWidget(new QLabel(QObject::tr("Motor device path")),0,0);
        edit_motor = new QLineEdit;
        edit_motor->setText(settings.value(SETTINGS_MOTOR_PATH).toString());
        layout->addWidget(edit_motor,0,1);
        button = new QToolButton;
        QObject::connect(button,SIGNAL(clicked()),this,SLOT(button_slot()));
        layout->addWidget(button,0,2);
        widget->setLayout(layout);
        return widget;
    }

    bool accept_config() {
        QSettings settings;
        settings.setValue(SETTINGS_MOTOR_PATH, edit_motor->text());
        return true;
    }
    bool is_configurable() {return true;}

private slots:
    void button_slot(){
        edit_motor->setText(QFileDialog::getOpenFileName(NULL, tr("Choose device"),"/dev", ""));
    }
private:
    QLineEdit *edit_motor;
    QToolButton *button;
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
    QString get_general_name(){ return "Temperature";}
    QString get_name(int n){Q_UNUSED(n);  return "Temperature"; }
    QString get_units(int n) {Q_UNUSED(n); return QString::fromUtf8("°C"); }
    double get_lower_bound(int n) {Q_UNUSED(n); return 0;}
    double get_upper_bound(int n) {Q_UNUSED(n); return 0;}
    double get_smaller_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_start(int n) {Q_UNUSED(n); return 0;}
};
class Virtual_TemperatureGUI: public VariableGUI {
public:
    Virtual_TemperatureGUI() {meta = new Virtual_TemperatureMeta();}
    QWidget* get_config_widget() {return NULL;}
    bool accept_config() {return true;}
    bool is_configurable() {return false;}
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
    QString get_general_name(){ return "Time";}
    QString get_name(int n){Q_UNUSED(n);  return "Time"; }
    QString get_units(int n) {Q_UNUSED(n); return "s"; }
    double get_lower_bound(int n) {Q_UNUSED(n); return 0;}
    double get_upper_bound(int n) {Q_UNUSED(n); return 0;}
    double get_smaller_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_step(int n) {Q_UNUSED(n); return 0;}
    double get_default_start(int n) {Q_UNUSED(n); return 0;}
};
class Virtual_TimeGUI: public VariableGUI {
public:
    Virtual_TimeGUI() {meta = new Virtual_TimeMeta();}
    QWidget* get_config_widget() {return NULL;}

    bool accept_config() {return true;}
    bool is_configurable() {return false;}
private:
    QLineEdit *edit_beta;
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
