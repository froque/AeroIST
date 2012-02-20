#include "temperature.h"

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

#include "../arduino-serial.h"

#include <QSettings>

#define TEMPERATURE_SENSITIVITY 100.0

#define ARDUINO_ANALOG_REF 5.0
#define SETTINGS_ARDUINO_PATH "arduino_path"
#define SETTINGS_ARDUINO_PATH_DEFAULT "/dev/ttyUSB0"

bool TemperatureMeta::is_controlable() {
    return false;
}
bool TemperatureMeta::has_zero() {
    return false;
}
int TemperatureMeta::get_num() {
    return 1;
}
QString TemperatureMeta::get_general_name(){
    return "Temperature";
}
QString TemperatureMeta::get_name(int n){
    Q_UNUSED(n);
    return "Temperature";
}
QString TemperatureMeta::get_units(int n) {
    Q_UNUSED(n);
    return QString::fromUtf8("°C");
}
double TemperatureMeta::get_lower_bound(int n) {
    Q_UNUSED(n);
    return 0;
}
double TemperatureMeta::get_upper_bound(int n) {
    Q_UNUSED(n);
    return 0;
}
double TemperatureMeta::get_smaller_step(int n) {
    Q_UNUSED(n);
    return 0;
}
double TemperatureMeta::get_default_step(int n) {
    Q_UNUSED(n);
    return 0;
}
double TemperatureMeta::get_default_start(int n) {
    Q_UNUSED(n);
    return 0;
}


TemperaturePreferences::TemperaturePreferences() {
    meta = new TemperatureMeta();
}
QWidget* TemperaturePreferences::get_widget() {
    return NULL;
}
bool TemperaturePreferences::accept_config() {
    return true;
}
bool TemperaturePreferences::is_configurable() {
    return false;
}


TemperatureModel::TemperatureModel(){
    meta = new TemperatureMeta;
}
int TemperatureModel::get_size() {
    return contents.size();
}
double TemperatureModel::get_value(int n,int row) {
    Q_UNUSED(n);
    return contents.value(row);
}
QVector<double> TemperatureModel::get_vector(int n) {
    Q_UNUSED(n);
    return contents;
}
void TemperatureModel::set_value(int n ,int row, double value) {
    Q_UNUSED(n);
    contents.replace(row,value);
}
void TemperatureModel::insert_value(int n, int row, int count, double value) {
    Q_UNUSED(n);
    contents.insert(row,count,value);
}
void TemperatureModel::append_value(int n, double value) {
    Q_UNUSED(n);
    contents.append(value);
}
void TemperatureModel::set_zero(QVector<double> zero) {
    Q_UNUSED(zero);
}
QVector<double> TemperatureModel::get_zero() {
    return QVector<double>();
}
QWidget* TemperatureModel::view_get_widget(){
    return NULL;
}
QWidget* TemperatureModel::measurement_get_widget(){
    return NULL;
}
bool TemperatureModel::measurement_accept_config(VariableModel *m){
    Q_UNUSED(m);
    return true;
}
bool TemperatureModel::measurement_is_configurable(){
    return false;
}
void TemperatureModel::save_xml(QDomElement root){
    Q_UNUSED(root);
}
void TemperatureModel::load_xml(QDomElement root){
    Q_UNUSED(root);
}

TemperatureHardware::TemperatureHardware() {
    meta = new TemperatureMeta;
    temp=0;
    temp_raw=0;
    QSettings settings;
    arduinofd = serialport_init(settings.value(SETTINGS_ARDUINO_PATH).toString().toStdString().c_str(),SERIALRATE);
}

TemperatureHardware::~TemperatureHardware() {
    close(arduinofd);
}
void TemperatureHardware::read() {
    char buffer_read[256]="", buffer_aux[256];
    char buffer[] = "T0xxxx";
    bool sucess=false;

    sprintf(buffer_aux,"\"%s\"",buffer);
    while (sucess == false){
        serialport_flush(arduinofd);
        if( serialport_write(arduinofd, buffer) == -1){
            perror("temperature writing");
        }

        serialport_read_until(arduinofd, buffer_read, '\n');

        if (strncmp(buffer_aux,buffer_read,3)==0){
            strncpy(buffer_aux,buffer_read + 3,4);
            temp_raw = atoi(buffer_aux);
            sucess=true;
        }
    }
    // 3.3V
    // 10 bits = 1024
    // 100 ºC/V = 0,01 V/ºC
    temp = temp_raw * ARDUINO_ANALOG_REF * TEMPERATURE_SENSITIVITY /1024.0;
}


double TemperatureHardware::get_value(int n) {
    Q_UNUSED(n);
    return temp;
}
void TemperatureHardware::set_value(int n ,double value) {
    Q_UNUSED(n);
    Q_UNUSED(value);
}
bool TemperatureHardware::isReady(void) {
    return true;
}
bool TemperatureHardware::has_set_final() {
    return meta->is_controlable() && false;
}
void TemperatureHardware::set_final() {
}
void TemperatureHardware::set_zero(QVector<double> zero) {
    Q_UNUSED(zero);
}



VariableMeta* TemperatureFactory::CreateVariableMeta() {
    return new TemperatureMeta;
}
VariablePreferences* TemperatureFactory::CreateVariableGUI() {
    return new TemperaturePreferences;
}
VariableModel* TemperatureFactory::CreateVariableModel() {
    return new TemperatureModel;
}
VariableHardware* TemperatureFactory::CreateVariableHardware(VariableModel *v) {
    Q_UNUSED(v);
    return new TemperatureHardware;
}

Q_EXPORT_PLUGIN2(temperature, TemperatureFactory);

