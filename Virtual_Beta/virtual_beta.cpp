#include "virtual_beta.h"

#define DEFAULT_BETA_STEP   5
#define DEFAULT_BETA_START  0
#define ANGLEZERO_BETA 80751.0
#define ANGLESENSITIVITY_BETA 0.00211000
#define ANGLEMAX_BETA 180.0
#define ANGLEMINSTEP_BETA 0.2
#define PRECISION_BETA 0.05


#define SETTINGS_BETA_PATH "beta_path"
#define SETTINGS_BETA_PATH_DEFAULT "/dev/angle_beta"


bool BetaMeta::is_controlable() {return true;}
bool BetaMeta::has_zero() {return false;}
int BetaMeta::get_num() {return 1;}
QString BetaMeta::get_general_name(){ return "Beta";}
QString BetaMeta::get_general_name_tr(){
    return QString(QObject::tr("Beta"));
}
QString BetaMeta::get_name(int n){Q_UNUSED(n);  return "Beta"; }
QString BetaMeta::get_name_tr(int n){
    Q_UNUSED(n);
    return QString(QObject::tr("Beta"));
}
QString BetaMeta::get_units(int n) {
    Q_UNUSED(n);
    return QString(QObject::trUtf8("º"));
}
QString BetaMeta::get_raw_units(int n) {
    Q_UNUSED(n);
    return QString(QObject::tr("step"));
}
double BetaMeta::get_lower_bound(int n) {Q_UNUSED(n); return -ANGLEMAX_BETA;}
double BetaMeta::get_upper_bound(int n) {Q_UNUSED(n); return ANGLEMAX_BETA;}
double BetaMeta::get_smaller_step(int n) {Q_UNUSED(n); return ANGLEMINSTEP_BETA;}
double BetaMeta::get_default_step(int n) {Q_UNUSED(n); return DEFAULT_BETA_STEP;}
double BetaMeta::get_default_start(int n) {Q_UNUSED(n); return DEFAULT_BETA_START;}


BetaPreferences::BetaPreferences() {
    meta = new BetaMeta();
}
BetaPreferences::~BetaPreferences(){
    delete meta;
}
QWidget* BetaPreferences::get_widget() {
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    layout->addWidget(new QLabel(QObject::tr("Beta device")),0,0);
    edit_beta = new QLineEdit;
    edit_beta->setText(settings.value(SETTINGS_BETA_PATH,SETTINGS_BETA_PATH_DEFAULT).toString());
    layout->addWidget(edit_beta,0,1);
    widget->setLayout(layout);
    return widget;
}

bool BetaPreferences::accept_config() {
    QSettings settings;
    settings.setValue(SETTINGS_BETA_PATH, edit_beta->text());
    return true;
}
bool BetaPreferences::is_configurable() {return true;}


BetaModel::BetaModel(){
    meta = new BetaMeta;
}
BetaModel::~BetaModel(){
    delete meta;
}
int BetaModel::get_size() {return contents.size();}
double BetaModel::get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
QVector<double> BetaModel::get_vector(int n) {Q_UNUSED(n); return contents;}
void BetaModel::set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.insert(row,value);}
void BetaModel::insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
void BetaModel::append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
void BetaModel::set_zero(QVector<double> zero) {Q_UNUSED(zero);}
QVector<double> BetaModel::get_zero() {return QVector<double> ();}
QWidget* BetaModel::view_get_widget(){ return NULL;}
QWidget* BetaModel::measurement_get_widget(){return NULL;}
bool BetaModel::measurement_accept_config(VariableModel *m){Q_UNUSED(m); return true;}
bool BetaModel::measurement_is_configurable(){return false;}
void BetaModel::save_xml(QDomElement root){Q_UNUSED(root);}
void BetaModel::load_xml(QDomElement root){Q_UNUSED(root);}
void BetaModel::set_raw_value(int n, int row, double value){
    Q_UNUSED(n);
    raw.replace(row,value);
}
double BetaModel::get_raw_value(int n, int row){
    Q_UNUSED(n);
    return raw.value(row);
}
void BetaModel::insert_raw_value(int n, int row, int count, double value) {Q_UNUSED(n); raw.insert(row,count,value);}
void BetaModel::append_raw_value(int n, double value) {Q_UNUSED(n);  raw.append(value);}

BetaHardware::BetaHardware () {
    meta = new BetaMeta;
    value=0;
    control_set=false;
}
BetaHardware::~BetaHardware(){
    delete meta;
}
void BetaHardware::read() {if(control_set==false){ raw = (1.0 * qrand() / RAND_MAX); value = 20 *raw + 2;}}
double BetaHardware::get_value(int n) {Q_UNUSED(n); return value;}
double BetaHardware::get_raw_value(int n) {Q_UNUSED(n); return raw;}
void BetaHardware::set_value(int n ,double value) {Q_UNUSED(n);  control_set=true; this->value = value; this->raw = value/20.0 - 2;}
bool BetaHardware::isReady(void) {return true;}
bool BetaHardware::has_set_final() {return meta->is_controlable() && false;}
void BetaHardware::set_final() {}
void BetaHardware::set_zero(QVector<double> zero) {Q_UNUSED(zero);}


VariableMeta* BetaFactory::CreateVariableMeta() { return new BetaMeta;}
VariablePreferences* BetaFactory::CreateVariableGUI() { return new BetaPreferences;}
VariableModel* BetaFactory::CreateVariableModel() { return new BetaModel;}
VariableHardware* BetaFactory::CreateVariableHardware(VariableModel *v) {Q_UNUSED(v); return new BetaHardware;}

Q_EXPORT_PLUGIN2(22_beta, BetaFactory);
