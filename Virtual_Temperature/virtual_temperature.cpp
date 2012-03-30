#include "virtual_temperature.h"

bool TemperatureMeta::is_controlable() {return false;}
bool TemperatureMeta::has_zero() {return false;}
int TemperatureMeta::get_num() {return 1;}
QString TemperatureMeta::get_general_name(){ return "Temperature";}
QString TemperatureMeta::get_name(int n){Q_UNUSED(n);  return "Temperature"; }
QString TemperatureMeta::get_units(int n) {Q_UNUSED(n); return QString::fromUtf8("°C"); }
double TemperatureMeta::get_lower_bound(int n) {Q_UNUSED(n); return 0;}
double TemperatureMeta::get_upper_bound(int n) {Q_UNUSED(n); return 0;}
double TemperatureMeta::get_smaller_step(int n) {Q_UNUSED(n); return 0;}
double TemperatureMeta::get_default_step(int n) {Q_UNUSED(n); return 0;}
double TemperatureMeta::get_default_start(int n) {Q_UNUSED(n); return 0;}


TemperaturePreferences::TemperaturePreferences() {meta = new TemperatureMeta();}
QWidget* TemperaturePreferences::get_widget() {return NULL;}
bool TemperaturePreferences::accept_config() {return true;}
bool TemperaturePreferences::is_configurable() {return false;}


TemperatureModel::TemperatureModel(){meta = new TemperatureMeta;}
int TemperatureModel::get_size() {return contents.size();}
double TemperatureModel::get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
QVector<double> TemperatureModel::get_vector(int n) {Q_UNUSED(n); return contents;}
void TemperatureModel::set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
void TemperatureModel::insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
void TemperatureModel::append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
void TemperatureModel::set_zero(QVector<double> zero) {Q_UNUSED(zero);}
QVector<double> TemperatureModel::get_zero() {return QVector<double>();}
QWidget* TemperatureModel::view_get_widget(){ return NULL;}
QWidget* TemperatureModel::measurement_get_widget(){return NULL;}
bool TemperatureModel::measurement_accept_config(VariableModel *m){Q_UNUSED(m); return true;}
bool TemperatureModel::measurement_is_configurable(){return false;}
void TemperatureModel::save_xml(QDomElement root){Q_UNUSED(root);}
void TemperatureModel::load_xml(QDomElement root){Q_UNUSED(root);}
void TemperatureModel::set_raw_value(int n, int row, double value){
    Q_UNUSED(n);
    raw.replace(row,value);
}
double TemperatureModel::get_raw_value(int n, int row){
    Q_UNUSED(n);
    return raw.value(row);
}
void TemperatureModel::insert_raw_value(int n, int row, int count, double value) {Q_UNUSED(n); raw.insert(row,count,value);}
void TemperatureModel::append_raw_value(int n, double value) {Q_UNUSED(n);  raw.append(value);}

TemperatureHardware::TemperatureHardware() {meta = new TemperatureMeta;}
void TemperatureHardware::read() { raw = (1.0* qrand() / RAND_MAX); value = -10.0 * raw +1;}
double TemperatureHardware::get_value(int n) {Q_UNUSED(n); return value;}
double TemperatureHardware::get_raw_value(int n) {Q_UNUSED(n); return raw;}
void TemperatureHardware::set_value(int n ,double value) {Q_UNUSED(n); Q_UNUSED(value);  }
bool TemperatureHardware::isReady(void) {return true;}
bool TemperatureHardware::has_set_final() {return meta->is_controlable() && false;}
void TemperatureHardware::set_final() {}
void TemperatureHardware::set_zero(QVector<double> zero) {Q_UNUSED(zero);}



VariableMeta* TemperatureFactory::CreateVariableMeta() { return new TemperatureMeta;}
VariablePreferences* TemperatureFactory::CreateVariableGUI() { return new TemperaturePreferences;}
VariableModel* TemperatureFactory::CreateVariableModel() { return new TemperatureModel;}
VariableHardware* TemperatureFactory::CreateVariableHardware(VariableModel *v) { Q_UNUSED(v); return new TemperatureHardware;}

Q_EXPORT_PLUGIN2(50_temperature, TemperatureFactory);
