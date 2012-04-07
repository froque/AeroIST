#include "virtual_alpha.h"

#define DEFAULT_ALPHA_STEP  1
#define DEFAULT_ALPHA_START 0
#define ANGLEZERO_ALPHA 730303.0
#define ANGLESENSITIVITY_ALPHA 0.0000009007
#define ANGLEMAX_ALPHA 30.0
#define ANGLEMINSTEP_ALPHA 0.1
#define PRECISION_ALPHA 0.03

#define SETTINGS_ALPHA_PATH "alpha_path"
#define SETTINGS_ALPHA_PATH_DEFAULT "/dev/angle_alpha"


bool AlphaMeta::is_controlable() {return true;}
bool AlphaMeta::has_zero() {return false;}
int AlphaMeta::get_num() {return 1;}
QString AlphaMeta::get_general_name(){ return "Alpha";}
QString AlphaMeta::get_general_name_tr(){
    return QString(QObject::tr("Alpha"));
}
QString AlphaMeta::get_name(int n){Q_UNUSED(n);  return "Alpha"; }
QString AlphaMeta::get_name_tr(int n){
    Q_UNUSED(n);
    return QString(QObject::tr("Alpha"));
}
QString AlphaMeta::get_units(int n) {
    Q_UNUSED(n);
    return QString(QObject::trUtf8("º"));
}
double AlphaMeta::get_lower_bound(int n) {Q_UNUSED(n); return -ANGLEMAX_ALPHA;}
double AlphaMeta::get_upper_bound(int n) {Q_UNUSED(n); return ANGLEMAX_ALPHA;}
double AlphaMeta::get_smaller_step(int n) {Q_UNUSED(n); return ANGLEMINSTEP_ALPHA;}
double AlphaMeta::get_default_step(int n) {Q_UNUSED(n); return DEFAULT_ALPHA_STEP;}
double AlphaMeta::get_default_start(int n) {Q_UNUSED(n); return DEFAULT_ALPHA_START;}


AlphaPreferences::AlphaPreferences() {meta = new AlphaMeta();}
QWidget* AlphaPreferences::get_widget() {
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    layout->addWidget(new QLabel(QObject::tr("Alpha device")),0,0);
    edit_alpha = new QLineEdit;
    edit_alpha->setText(settings.value(SETTINGS_ALPHA_PATH,SETTINGS_ALPHA_PATH_DEFAULT).toString());
    layout->addWidget(edit_alpha,0,1);
    widget->setLayout(layout);
    return widget;
}

bool AlphaPreferences::accept_config() {
    QSettings settings;
    settings.setValue(SETTINGS_ALPHA_PATH, edit_alpha->text());
    return true;
}
bool AlphaPreferences::is_configurable() {return true;}

AlphaModel::AlphaModel(){meta = new AlphaMeta;}
int AlphaModel::get_size() {return contents.size();}
double AlphaModel::get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
QVector<double> AlphaModel::get_vector(int n) {Q_UNUSED(n); return contents;}
void AlphaModel::set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
void AlphaModel::insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
void AlphaModel::append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
void AlphaModel::set_zero(QVector<double> zero) {Q_UNUSED(zero);}
QVector<double> AlphaModel::get_zero() {return QVector<double>();}
QWidget* AlphaModel::view_get_widget(){ return NULL;}
QWidget* AlphaModel::measurement_get_widget(){return NULL;}
bool AlphaModel::measurement_accept_config(VariableModel *m){Q_UNUSED(m); return true;}

bool AlphaModel::measurement_is_configurable(){return false;}
void AlphaModel::save_xml(QDomElement root){Q_UNUSED(root);}
void AlphaModel::load_xml(QDomElement root){Q_UNUSED(root);}
void AlphaModel::set_raw_value(int n, int row, double value){
    Q_UNUSED(n);
    raw.replace(row,value);
}
double AlphaModel::get_raw_value(int n, int row){
    Q_UNUSED(n);
    return raw.value(row);
}
void AlphaModel::insert_raw_value(int n, int row, int count, double value) {Q_UNUSED(n); raw.insert(row,count,value);}
void AlphaModel::append_raw_value(int n, double value) {Q_UNUSED(n);  raw.append(value);}



AlphaHardware::AlphaHardware(){meta = new AlphaMeta; value=0; control_set=false;}
void AlphaHardware::read() {if(control_set==false){ raw = (1.0 * qrand() / RAND_MAX); value = 100 *raw + 10;}}
double AlphaHardware::get_value(int n) {Q_UNUSED(n); return value;}
double AlphaHardware::get_raw_value(int n) {Q_UNUSED(n); return raw;}
void AlphaHardware::set_value(int n ,double value) {Q_UNUSED(n);  control_set=true; this->value = value;this->raw = value/100.0 - 10;}
bool AlphaHardware::isReady(void) {return true;}
bool AlphaHardware::has_set_final() {return meta->is_controlable() && false;}
void AlphaHardware::set_final() {}
void AlphaHardware::set_zero(QVector<double> zero) {Q_UNUSED(zero);}


VariableMeta* AlphaFactory::CreateVariableMeta() { return new AlphaMeta;}
VariablePreferences* AlphaFactory::CreateVariableGUI() { return new AlphaPreferences;}
VariableModel* AlphaFactory::CreateVariableModel() { return new AlphaModel;}
VariableHardware* AlphaFactory::CreateVariableHardware(VariableModel *v) {Q_UNUSED(v); return new AlphaHardware;}

Q_EXPORT_PLUGIN2(21_alpha, AlphaFactory);
