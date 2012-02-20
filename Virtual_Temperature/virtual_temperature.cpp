#include "virtual_temperature.h"

bool Virtual_TemperatureMeta::is_controlable() {return false;}
bool Virtual_TemperatureMeta::has_zero() {return false;}
int Virtual_TemperatureMeta::get_num() {return 1;}
QString Virtual_TemperatureMeta::get_general_name(){ return "Temperature";}
QString Virtual_TemperatureMeta::get_name(int n){Q_UNUSED(n);  return "Temperature"; }
QString Virtual_TemperatureMeta::get_units(int n) {Q_UNUSED(n); return QString::fromUtf8("°C"); }
double Virtual_TemperatureMeta::get_lower_bound(int n) {Q_UNUSED(n); return 0;}
double Virtual_TemperatureMeta::get_upper_bound(int n) {Q_UNUSED(n); return 0;}
double Virtual_TemperatureMeta::get_smaller_step(int n) {Q_UNUSED(n); return 0;}
double Virtual_TemperatureMeta::get_default_step(int n) {Q_UNUSED(n); return 0;}
double Virtual_TemperatureMeta::get_default_start(int n) {Q_UNUSED(n); return 0;}


Virtual_TemperatureGUI::Virtual_TemperatureGUI() {meta = new Virtual_TemperatureMeta();}
QWidget* Virtual_TemperatureGUI::get_widget() {return NULL;}
bool Virtual_TemperatureGUI::accept_config() {return true;}
bool Virtual_TemperatureGUI::is_configurable() {return false;}


Virtual_TemperatureModel::Virtual_TemperatureModel(){meta = new Virtual_TemperatureMeta;}
int Virtual_TemperatureModel::get_size() {return contents.size();}
double Virtual_TemperatureModel::get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
QVector<double> Virtual_TemperatureModel::get_vector(int n) {Q_UNUSED(n); return contents;}
void Virtual_TemperatureModel::set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
void Virtual_TemperatureModel::insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
void Virtual_TemperatureModel::append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
void Virtual_TemperatureModel::set_zero(QVector<double> zero) {Q_UNUSED(zero);}
QVector<double> Virtual_TemperatureModel::get_zero() {return QVector<double>();}
QWidget* Virtual_TemperatureModel::view_get_widget(){ return NULL;}
QWidget* Virtual_TemperatureModel::measurement_get_widget(){return NULL;}
bool Virtual_TemperatureModel::measurement_accept_config(VariableModel *m){Q_UNUSED(m); return true;}
bool Virtual_TemperatureModel::measurement_is_configurable(){return false;}
void Virtual_TemperatureModel::save_xml(QDomElement root){Q_UNUSED(root);}
void Virtual_TemperatureModel::load_xml(QDomElement root){Q_UNUSED(root);}

Virtual_TemperatureHardware::Virtual_TemperatureHardware() {meta = new Virtual_TemperatureMeta;}
void Virtual_TemperatureHardware::read() { value = -10.0 * qrand() / RAND_MAX;}
double Virtual_TemperatureHardware::get_value(int n) {Q_UNUSED(n); return value;}
void Virtual_TemperatureHardware::set_value(int n ,double value) {Q_UNUSED(n); Q_UNUSED(value);  }
bool Virtual_TemperatureHardware::isReady(void) {return true;}
bool Virtual_TemperatureHardware::has_set_final() {return meta->is_controlable() && false;}
void Virtual_TemperatureHardware::set_final() {}
void Virtual_TemperatureHardware::set_zero(QVector<double> zero) {Q_UNUSED(zero);}



VariableMeta* TemperatureFactory::CreateVariableMeta() { return new Virtual_TemperatureMeta;}
VariablePreferences* TemperatureFactory::CreateVariableGUI() { return new Virtual_TemperatureGUI;}
VariableModel* TemperatureFactory::CreateVariableModel() { return new Virtual_TemperatureModel;}
VariableHardware* TemperatureFactory::CreateVariableHardware(VariableModel *v) { Q_UNUSED(v); return new Virtual_TemperatureHardware;}

Q_EXPORT_PLUGIN2(virtual_temperature, TemperatureFactory);
