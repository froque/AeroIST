#include "virtual_motor.h"



bool Virtual_MotorMeta::is_controlable() {return true;}
bool Virtual_MotorMeta::has_zero() {return false;}
int Virtual_MotorMeta::get_num() {return 1;}
QString Virtual_MotorMeta::get_general_name(){ return "Motor";}
QString Virtual_MotorMeta::get_name(int n){Q_UNUSED(n);  return "Motor"; }
QString Virtual_MotorMeta::get_units(int n) {Q_UNUSED(n); return "%"; }
double Virtual_MotorMeta::get_lower_bound(int n) {Q_UNUSED(n); return MOTOR_MIN;}
double Virtual_MotorMeta::get_upper_bound(int n) {Q_UNUSED(n); return MOTOR_MAX;}
double Virtual_MotorMeta::get_smaller_step(int n) {Q_UNUSED(n); return MOTOR_MIN_STEP;}
double Virtual_MotorMeta::get_default_step(int n) {Q_UNUSED(n); return DEFAULT_MOTOR_STEP;}
double Virtual_MotorMeta::get_default_start(int n) {Q_UNUSED(n); return 0;}



Virtual_MotorGUI::Virtual_MotorGUI() {meta = new Virtual_MotorMeta();}
QWidget* Virtual_MotorGUI::get_config_widget() {
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

bool Virtual_MotorGUI::accept_config() {
    QSettings settings;
    settings.setValue(SETTINGS_MOTOR_PATH, edit_motor->text());
    return true;
}
bool Virtual_MotorGUI::is_configurable() {return true;}


void Virtual_MotorGUI::button_slot(){
    edit_motor->setText(QFileDialog::getOpenFileName(NULL, tr("Choose device"),"/dev", ""));
}




Virtual_MotorModel::Virtual_MotorModel(){meta = new Virtual_MotorMeta;}
int Virtual_MotorModel::get_size() {return contents.size();}
double Virtual_MotorModel::get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
QVector<double> Virtual_MotorModel::get_vector(int n) {Q_UNUSED(n); return contents;}
void Virtual_MotorModel::set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
void Virtual_MotorModel::insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
void Virtual_MotorModel::append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
void Virtual_MotorModel::set_zero(QVector<double> zero) {Q_UNUSED(zero);}
QVector<double> Virtual_MotorModel::get_zero() {return QVector<double>();}


Virtual_MotorHardware::Virtual_MotorHardware () {meta = new Virtual_MotorMeta; value=0; control_set=false;}
void Virtual_MotorHardware::read() {if(control_set==false){ value = -10.0 * qrand() / RAND_MAX;}}
double Virtual_MotorHardware::get_value(int n) {Q_UNUSED(n); return value;}
void Virtual_MotorHardware::set_value(int n ,double value) {Q_UNUSED(n);  control_set=true; this->value = value;}
QString Virtual_MotorHardware::get_name(int n) {Q_UNUSED(n); return "Motor";}
bool Virtual_MotorHardware::isReady(void) {return true;}
bool Virtual_MotorHardware::has_set_final() {return meta->is_controlable() && true;}
void Virtual_MotorHardware::set_final() {set_value(0,0);}
void Virtual_MotorHardware::set_zero(QVector<double> zero) {Q_UNUSED(zero);}


VariableMeta* MotorFactory::CreateVariableMeta() { return new Virtual_MotorMeta;}
VariableGUI* MotorFactory::CreateVariableGUI() { return new Virtual_MotorGUI;}
VariableModel* MotorFactory::CreateVariableModel() { return new Virtual_MotorModel;}
VariableHardware* MotorFactory::CreateVariableHardware() { return new Virtual_MotorHardware;}


Q_EXPORT_PLUGIN2(virtual_motor, MotorFactory);
