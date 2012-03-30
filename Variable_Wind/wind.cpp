#include "wind.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>   /* Standard types */
#include "arduino-serial.h"
#include <QSettings>

#define WIND_SENSITIVITY 3.98
#define MMH2O_TO_PASCAL 9.80665

#define ARDUINO_ANALOG_REF 5.0
#define SETTINGS_ARDUINO_PATH "arduino_path"
#define SETTINGS_ARDUINO_PATH_DEFAULT "/dev/ttyUSB0"

bool WindMeta::is_controlable() {
    return false;
}
bool WindMeta::has_zero() {
    return false;
}
int WindMeta::get_num() {
    return 1;
}
QString WindMeta::get_general_name(){
    return "Wind";
}
QString WindMeta::get_name(int n){
    Q_UNUSED(n);
    return "Wind";
}
QString WindMeta::get_units(int n) {
    Q_UNUSED(n);
    return "mmH20";
}
double WindMeta::get_lower_bound(int n) {
    Q_UNUSED(n);
    return 0;
}
double WindMeta::get_upper_bound(int n) {
    Q_UNUSED(n);
    return 0;
}
double WindMeta::get_smaller_step(int n) {
    Q_UNUSED(n);
    return 0;
}
double WindMeta::get_default_step(int n) {
    Q_UNUSED(n);
    return 0;
}
double WindMeta::get_default_start(int n) {
    Q_UNUSED(n);
    return 0;
}


WindPreferences::WindPreferences() {
    meta = new WindMeta();
}
QWidget* WindPreferences::get_widget() {
    return NULL;
}
bool WindPreferences::accept_config() {
    return true;
}
bool WindPreferences::is_configurable() {
    return true;
}


WindModel::WindModel(){
    meta = new WindMeta;
}
int WindModel::get_size() {
    return contents.size();
}
double WindModel::get_value(int n,int row) {
    Q_UNUSED(n);
    return contents.value(row);
}
QVector<double> WindModel::get_vector(int n) {
    Q_UNUSED(n);
    return contents;
}
void WindModel::set_value(int n ,int row, double value) {
    Q_UNUSED(n);
    contents.replace(row,value);
}
void WindModel::insert_value(int n, int row, int count, double value) {
    Q_UNUSED(n);
    contents.insert(row,count,value);
}
void WindModel::append_value(int n, double value) {
    Q_UNUSED(n);
    contents.append(value);
}
double WindModel::get_raw_value(int n,int row){
    Q_UNUSED(n);
    return raw.value(row);
}
void WindModel::set_raw_value(int n ,int row, double value){
    Q_UNUSED(n);
    raw.replace(row,value);
}
void WindModel::insert_raw_value(int n, int row, int count, double value) {
    Q_UNUSED(n);
    raw.insert(row,count,value);
}
void WindModel::append_raw_value(int n, double value) {
    Q_UNUSED(n);
    raw.append(value);
}
void WindModel::set_zero(QVector<double> zero) {
    Q_UNUSED(zero);
}
QVector<double> WindModel::get_zero() {
    return QVector<double>();
}
QWidget* WindModel::view_get_widget(){
    return NULL;
}
QWidget* WindModel::measurement_get_widget(){
    return NULL;
}
bool WindModel::measurement_accept_config(VariableModel *m){
    Q_UNUSED(m);
    return true;
}
bool WindModel::measurement_is_configurable(){
    return false;
}
void WindModel::save_xml(QDomElement root){
    Q_UNUSED(root);
}
void WindModel::load_xml(QDomElement root){
    Q_UNUSED(root);
}


WindHardware::WindHardware() {
    meta = new WindMeta;
    wind=0;
    wind_raw=0;
    QSettings settings;
    arduinofd = serialport_init(settings.value(SETTINGS_ARDUINO_PATH).toString().toStdString().c_str(),SERIALRATE);
}
WindHardware::~WindHardware() {
  close(arduinofd);
}
void WindHardware::read() {
    char buffer_read[256]="", buffer_aux[256];
    char buffer[] = "A5xxxx";               // read from analog 5
    bool sucess=false;

    sprintf(buffer_aux,"\"%s\"",buffer);
    while (sucess == false){
        serialport_flush(arduinofd);
        if( serialport_write(arduinofd, buffer) == -1){
            perror("Wind writing");
        }

        serialport_read_until(arduinofd, buffer_read, '\n');

        if (strncmp(buffer_aux,buffer_read,3)==0){
            strncpy(buffer_aux,buffer_read + 3,4);
            wind_raw = atoi(buffer_aux);
            sucess=true;
        }
    }
    // 3.3V
    // 10 bits = 1024
    // 19.9 mm H20 = 5 V => 3.98 mm H20/V
    // 1 mm H20 = 9.80665 Pa
//    wind = wind_raw * ARDUINO_ANALOG_REF/1024.0 * WIND_SENSITIVITY * MMH2O_TO_PASCAL ;
    wind = wind_raw * ARDUINO_ANALOG_REF/1024.0 * WIND_SENSITIVITY ;
}
double WindHardware::get_value(int n) {
    Q_UNUSED(n);
    return wind;
}
void WindHardware::set_value(int n ,double value) {
    Q_UNUSED(n);
    Q_UNUSED(value);
}
double WindHardware::get_raw_value(int n){
    Q_UNUSED(n);
    return wind_raw;
}
bool WindHardware::isReady(void) {
    return true;
}
bool WindHardware::has_set_final() {
    return meta->is_controlable() && false;
}
void WindHardware::set_final() {
}
void WindHardware::set_zero(QVector<double> zero) {
    Q_UNUSED(zero);
}



VariableMeta* WindFactory::CreateVariableMeta() {
    return new WindMeta;
}
VariablePreferences* WindFactory::CreateVariableGUI() {
    return new WindPreferences;
}
VariableModel* WindFactory::CreateVariableModel() {
    return new WindModel;
}
VariableHardware* WindFactory::CreateVariableHardware(VariableModel *v) {
    Q_UNUSED(v);
    return new WindHardware;
}


Q_EXPORT_PLUGIN2(40_wind, WindFactory);

