#include "virtual_motor.h"

#define MOTOR_MIN    0
#define MOTOR_MAX    100 //Percentage
#define MOTOR_MIN_STEP 1
#define DEFAULT_MOTOR_STEP   1

#define SETTINGS_MOTOR_PATH "motor_path"
#define SETTINGS_MOTOR_PATH_DEFAULT "/dev/ttyS0"


bool MotorMeta::is_controlable() {return true;}
bool MotorMeta::has_zero() {return false;}
int MotorMeta::get_num() {return 1;}
QString MotorMeta::get_general_name(){ return "Motor";}
QString MotorMeta::get_name(int n){Q_UNUSED(n);  return "Motor"; }
QString MotorMeta::get_units(int n) {Q_UNUSED(n); return "%"; }
double MotorMeta::get_lower_bound(int n) {Q_UNUSED(n); return MOTOR_MIN;}
double MotorMeta::get_upper_bound(int n) {Q_UNUSED(n); return MOTOR_MAX;}
double MotorMeta::get_smaller_step(int n) {Q_UNUSED(n); return MOTOR_MIN_STEP;}
double MotorMeta::get_default_step(int n) {Q_UNUSED(n); return DEFAULT_MOTOR_STEP;}
double MotorMeta::get_default_start(int n) {Q_UNUSED(n); return 0;}

MotorPreferences::MotorPreferences() {meta = new MotorMeta();}
QWidget* MotorPreferences::get_widget() {
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    layout->addWidget(new QLabel(QObject::tr("Motor device path")),0,0);
    edit_motor = new QLineEdit;
    edit_motor->setText(settings.value(SETTINGS_MOTOR_PATH,SETTINGS_MOTOR_PATH_DEFAULT).toString());
    layout->addWidget(edit_motor,0,1);
    button = new QToolButton;
    QObject::connect(button,SIGNAL(clicked()),this,SLOT(button_slot()));
    layout->addWidget(button,0,2);
    widget->setLayout(layout);
    return widget;
}

bool MotorPreferences::accept_config() {
    QSettings settings;
    settings.setValue(SETTINGS_MOTOR_PATH, edit_motor->text());
    return true;
}
bool MotorPreferences::is_configurable() {return true;}


void MotorPreferences::button_slot(){
    edit_motor->setText(QFileDialog::getOpenFileName(NULL, tr("Choose device"),"/dev", ""));
}

MotorModel::MotorModel(){meta = new MotorMeta;}
int MotorModel::get_size() {return contents.size();}
double MotorModel::get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
QVector<double> MotorModel::get_vector(int n) {Q_UNUSED(n); return contents;}
void MotorModel::set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
void MotorModel::insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
void MotorModel::append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
void MotorModel::set_zero(QVector<double> zero) {Q_UNUSED(zero);}
QVector<double> MotorModel::get_zero() {return QVector<double>();}
QWidget* MotorModel::view_get_widget(){ return NULL;}
QWidget* MotorModel::measurement_get_widget(){return NULL;}
bool MotorModel::measurement_accept_config(VariableModel *m){Q_UNUSED(m); return true;}
bool MotorModel::measurement_is_configurable(){return false;}
void MotorModel::save_xml(QDomElement root){Q_UNUSED(root);}
void MotorModel::load_xml(QDomElement root){Q_UNUSED(root);}
void MotorModel::set_raw_value(int n, int row, double value){
    Q_UNUSED(n);
    raw.replace(row,value);
}
double MotorModel::get_raw_value(int n, int row){
    Q_UNUSED(n);
    return raw.value(row);
}
void MotorModel::insert_raw_value(int n, int row, int count, double value) {Q_UNUSED(n); raw.insert(row,count,value);}
void MotorModel::append_raw_value(int n, double value) {Q_UNUSED(n);  raw.append(value);}

MotorHardware::MotorHardware () {meta = new MotorMeta; value=0; control_set=false;}
void MotorHardware::read() {if(control_set==false){raw = (1.0 * qrand() / RAND_MAX); value = -30.0 * raw + 5;}}
double MotorHardware::get_value(int n) {Q_UNUSED(n); return value;}
double MotorHardware::get_raw_value(int n) {Q_UNUSED(n); return raw;}
void MotorHardware::set_value(int n ,double value) {Q_UNUSED(n);  control_set=true; this->value = value;this->raw = value/-30.0 - 5;}
QString MotorHardware::get_name(int n) {Q_UNUSED(n); return "Motor";}
bool MotorHardware::isReady(void) {return true;}
bool MotorHardware::has_set_final() {return meta->is_controlable() && true;}
void MotorHardware::set_final() {set_value(0,0);}
void MotorHardware::set_zero(QVector<double> zero) {Q_UNUSED(zero);}


VariableMeta* MotorFactory::CreateVariableMeta() { return new MotorMeta;}
VariablePreferences* MotorFactory::CreateVariableGUI() { return new MotorPreferences;}
VariableModel* MotorFactory::CreateVariableModel() { return new MotorModel;}
VariableHardware* MotorFactory::CreateVariableHardware(VariableModel *v) {Q_UNUSED(v); return new MotorHardware;}

Q_EXPORT_PLUGIN2(30_motor, MotorFactory);
