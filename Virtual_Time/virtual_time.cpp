#include "virtual_time.h"



bool Virtual_TimeMeta::is_controlable() {return false;}
bool Virtual_TimeMeta::has_zero() {return false;}
int Virtual_TimeMeta::get_num() {return 1;}
QString Virtual_TimeMeta::get_general_name(){ return "Time";}
QString Virtual_TimeMeta::get_name(int n){Q_UNUSED(n);  return "Time"; }
QString Virtual_TimeMeta::get_units(int n) {Q_UNUSED(n); return "s"; }
double Virtual_TimeMeta::get_lower_bound(int n) {Q_UNUSED(n); return 0;}
double Virtual_TimeMeta::get_upper_bound(int n) {Q_UNUSED(n); return 0;}
double Virtual_TimeMeta::get_smaller_step(int n) {Q_UNUSED(n); return 0;}
double Virtual_TimeMeta::get_default_step(int n) {Q_UNUSED(n); return 0;}
double Virtual_TimeMeta::get_default_start(int n) {Q_UNUSED(n); return 0;}




Virtual_TimeGUI::Virtual_TimeGUI() {meta = new Virtual_TimeMeta();}
QWidget* Virtual_TimeGUI::get_config_widget() {return NULL;}

bool Virtual_TimeGUI::accept_config() {return true;}
bool Virtual_TimeGUI::is_configurable() {return false;}


Virtual_TimeModel::Virtual_TimeModel(){meta = new Virtual_TimeMeta;}
int Virtual_TimeModel::get_size() {return contents.size();}
double Virtual_TimeModel::get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
QVector<double> Virtual_TimeModel::get_vector(int n) {Q_UNUSED(n); return contents;}
void Virtual_TimeModel::set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
void Virtual_TimeModel::insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
void Virtual_TimeModel::append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
void Virtual_TimeModel::set_zero(QVector<double> zero) {Q_UNUSED(zero);}
QVector<double> Virtual_TimeModel::get_zero() {return QVector<double>();}


VariableMeta* TimeFactory::CreateVariableMeta() { return new Virtual_TimeMeta;}
VariableGUI* TimeFactory::CreateVariableGUI() { return new Virtual_TimeGUI;}
VariableModel* TimeFactory::CreateVariableModel() { return new Virtual_TimeModel;}
//    VariableHardware* CreateVariableHardware() { return new Virtual_TimeHardware;}
VariableHardware* TimeFactory::CreateVariableHardware() { return NULL;}



Q_EXPORT_PLUGIN2(virtual_time, TimeFactory);
