#include "time.h"

bool TimeMeta::is_controlable() {
    return false;
}
bool TimeMeta::has_zero() {
    return false;
}
int TimeMeta::get_num() {
    return 1;
}
QString TimeMeta::get_general_name(){
    return "Time";
}
QString TimeMeta::get_name(int n){
    Q_UNUSED(n);
    return "Time";
}
QString TimeMeta::get_units(int n) {
    Q_UNUSED(n);
    return "s";
}
double TimeMeta::get_lower_bound(int n) {
    Q_UNUSED(n);
    return 0;
}
double TimeMeta::get_upper_bound(int n) {
    Q_UNUSED(n);
    return 0;
}
double TimeMeta::get_smaller_step(int n) {
    Q_UNUSED(n);
    return 0;
}
double TimeMeta::get_default_step(int n) {
    Q_UNUSED(n);
    return 0;
}
double TimeMeta::get_default_start(int n) {
    Q_UNUSED(n);
    return 0;
}


TimePreferences::TimePreferences() {
    meta = new TimeMeta();
}
QWidget* TimePreferences::get_widget() {
    return NULL;
}
bool TimePreferences::accept_config() {
    return true;
}
bool TimePreferences::is_configurable() {
    return false;
}


TimeModel::TimeModel(){
    meta = new TimeMeta;
}
int TimeModel::get_size() {
    return contents.size();
}
double TimeModel::get_value(int n,int row) {
    Q_UNUSED(n);
    return contents.value(row);
}
QVector<double> TimeModel::get_vector(int n) {
    Q_UNUSED(n);
    return contents;
}
void TimeModel::set_value(int n ,int row, double value) {
    Q_UNUSED(n);
    contents.replace(row,value);
}
void TimeModel::insert_value(int n, int row, int count, double value) {
    Q_UNUSED(n);
    contents.insert(row,count,value);
}
void TimeModel::append_value(int n, double value) {
    Q_UNUSED(n);
    contents.append(value);
}
double TimeModel::get_raw_value(int n,int row){
    Q_UNUSED(n);
    return raw.value(row);
}
void TimeModel::set_raw_value(int n ,int row, double value){
    Q_UNUSED(n);
    raw.replace(row,value);
}
void TimeModel::insert_raw_value(int n, int row, int count, double value) {
    Q_UNUSED(n);
    raw.insert(row,count,value);
}
void TimeModel::append_raw_value(int n, double value) {
    Q_UNUSED(n);
    raw.append(value);
}
void TimeModel::set_zero(QVector<double> zero) {
    Q_UNUSED(zero);
}
QVector<double> TimeModel::get_zero() {
    return QVector<double>();
}
QWidget* TimeModel::view_get_widget(){
    return NULL;
}
QWidget* TimeModel::measurement_get_widget(){
    return NULL;
}
bool TimeModel::measurement_accept_config(VariableModel *m){
    Q_UNUSED(m);
    return true;
}
bool TimeModel::measurement_is_configurable(){
    return false;
}
void TimeModel::save_xml(QDomElement root){
    Q_UNUSED(root);
}
void TimeModel::load_xml(QDomElement root){
    Q_UNUSED(root);
}


TimeHardware::TimeHardware(){
    meta = new TimeMeta;
    virgin = true;
}
void TimeHardware::read(){
    if (virgin == true){
        timer.start();
        virgin = false;
    }

    tempo_mili = timer.elapsed();
    tempo_s = tempo_mili/1000.0;
}
double TimeHardware::get_value(int n){
    Q_UNUSED(n);
    return tempo_s;
}
double TimeHardware::get_raw_value(int n){
    Q_UNUSED(n);
    return tempo_mili;
}
void TimeHardware::set_value(int n ,double value){
    Q_UNUSED(n);
    Q_UNUSED(value);
}
bool TimeHardware::isReady(void){
    return true;
}
bool TimeHardware::has_set_final(){
    return false;
}
void TimeHardware::set_final(){
}
void TimeHardware::set_zero(QVector<double> zero){
    Q_UNUSED(zero);
}


VariableMeta* TimeFactory::CreateVariableMeta() {
    return new TimeMeta;
}
VariablePreferences* TimeFactory::CreateVariableGUI() {
    return new TimePreferences;
}
VariableModel* TimeFactory::CreateVariableModel() {
    return new TimeModel;
}
VariableHardware* TimeFactory::CreateVariableHardware(VariableModel *v) {
    Q_UNUSED(v);
    return new TimeHardware;
}

Q_EXPORT_PLUGIN2(time, TimeFactory);

