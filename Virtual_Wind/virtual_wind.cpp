#include "virtual_wind.h"


bool Virtual_WindMeta::is_controlable() {return false;}
bool Virtual_WindMeta::has_zero() {return false;}
int Virtual_WindMeta::get_num() {return 1;}
QString Virtual_WindMeta::get_general_name(){ return "Wind";}
QString Virtual_WindMeta::get_name(int n){Q_UNUSED(n);  return "Wind"; }
QString Virtual_WindMeta::get_units(int n) {Q_UNUSED(n); return "mmH20"; }
double Virtual_WindMeta::get_lower_bound(int n) {Q_UNUSED(n); return 0;}
double Virtual_WindMeta::get_upper_bound(int n) {Q_UNUSED(n); return 0;}
double Virtual_WindMeta::get_smaller_step(int n) {Q_UNUSED(n); return 0;}
double Virtual_WindMeta::get_default_step(int n) {Q_UNUSED(n); return 0;}
double Virtual_WindMeta::get_default_start(int n) {Q_UNUSED(n); return 0;}


Virtual_WindGUI::Virtual_WindGUI() {meta = new Virtual_WindMeta();}
QWidget* Virtual_WindGUI::get_widget() {return NULL;}
bool Virtual_WindGUI::accept_config() {return true;}
bool Virtual_WindGUI::is_configurable() {return true;}


Virtual_WindModel::Virtual_WindModel(){meta = new Virtual_WindMeta;}
int Virtual_WindModel::get_size() {return contents.size();}
double Virtual_WindModel::get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
QVector<double> Virtual_WindModel::get_vector(int n) {Q_UNUSED(n); return contents;}
void Virtual_WindModel::set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
void Virtual_WindModel::insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
void Virtual_WindModel::append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
void Virtual_WindModel::set_zero(QVector<double> zero) {Q_UNUSED(zero);}
QVector<double> Virtual_WindModel::get_zero() {return QVector<double>();}
QWidget* Virtual_WindModel::view_get_widget(){ return NULL;}
QWidget* Virtual_WindModel::measurement_get_widget(){return NULL;}
bool Virtual_WindModel::measurement_accept_config(VariableModel *m){Q_UNUSED(m); return true;}
bool Virtual_WindModel::measurement_is_configurable(){return false;}
void Virtual_WindModel::save_xml(QDomElement root){Q_UNUSED(root);}
void Virtual_WindModel::load_xml(QDomElement root){Q_UNUSED(root);}


Virtual_WindHardware::Virtual_WindHardware() {meta = new Virtual_WindMeta;}
void Virtual_WindHardware::read() { value = -10.0 * qrand() / RAND_MAX;}
double Virtual_WindHardware::get_value(int n) {Q_UNUSED(n); return value;}
void Virtual_WindHardware::set_value(int n ,double value) {Q_UNUSED(n); Q_UNUSED(value);  }
bool Virtual_WindHardware::isReady(void) {return true;}
bool Virtual_WindHardware::has_set_final() {return meta->is_controlable() && false;}
void Virtual_WindHardware::set_final() {}
void Virtual_WindHardware::set_zero(QVector<double> zero) {Q_UNUSED(zero);}



VariableMeta* WindFactory::CreateVariableMeta() { return new Virtual_WindMeta;}
VariablePreferences* WindFactory::CreateVariableGUI() { return new Virtual_WindGUI;}
VariableModel* WindFactory::CreateVariableModel() { return new Virtual_WindModel;}
VariableHardware* WindFactory::CreateVariableHardware(VariableModel *v) { Q_UNUSED(v); return new Virtual_WindHardware;}


Q_EXPORT_PLUGIN2(virtual_wind, WindFactory);
