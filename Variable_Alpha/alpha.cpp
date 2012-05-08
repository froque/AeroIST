#include "alpha.h"

#include <fcntl.h>
#include <cmath>
#include <stdexcept>
#include <QSettings>

#include "botoneira.h"

#define DEFAULT_ALPHA_STEP  1
#define DEFAULT_ALPHA_START 0
#define ANGLEZERO_ALPHA 730303.0
#define ANGLESENSITIVITY_ALPHA 0.0000009007
#define ANGLEMAX_ALPHA 30.0
#define ANGLEMINSTEP_ALPHA 0.1
#define PRECISION_ALPHA 0.03

#define SETTINGS_ALPHA_PATH "alpha_path"
#define SETTINGS_ALPHA_PATH_DEFAULT "/dev/angle_alpha"

#define COMMAND_ON 1
#define COMMAND_OFF 0

#define ARDUINO_ANALOG_REF 5.0
#define SETTINGS_ARDUINO_PATH "arduino_path"
#define SETTINGS_ARDUINO_PATH_DEFAULT "/dev/arduino_duemilanove"
#define SETTINGS_ALPHA_PATH "alpha_path"
#define SETTINGS_ALPHA_PATH_DEFAULT "/dev/angle_alpha"

bool AlphaMeta::is_controlable() {
    return true;
}
bool AlphaMeta::has_zero() {
    return false;
}
int AlphaMeta::get_num() {
    return 1;
}
QString AlphaMeta::get_general_name(){
    return "Alpha";
}
QString AlphaMeta::get_general_name_tr(){
    return QString(QObject::tr("Alpha"));
}
QString AlphaMeta::get_name(int n){
    Q_UNUSED(n);
    return "Alpha";
}
QString AlphaMeta::get_name_tr(int n){
    Q_UNUSED(n);
    return QString(QObject::tr("Alpha"));
}
QString AlphaMeta::get_units(int n) {
    Q_UNUSED(n);
    return QString(QObject::trUtf8("º"));
}
double AlphaMeta::get_lower_bound(int n) {
    Q_UNUSED(n);
    return -ANGLEMAX_ALPHA;
}
double AlphaMeta::get_upper_bound(int n) {
    Q_UNUSED(n);
    return ANGLEMAX_ALPHA;
}
double AlphaMeta::get_smaller_step(int n) {
    Q_UNUSED(n);
    return ANGLEMINSTEP_ALPHA;
}
double AlphaMeta::get_default_step(int n) {
    Q_UNUSED(n);
    return DEFAULT_ALPHA_STEP;
}
double AlphaMeta::get_default_start(int n) {
    Q_UNUSED(n);
    return DEFAULT_ALPHA_START;
}


AlphaPreferences::AlphaPreferences() {
    meta = new AlphaMeta();
}
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
bool AlphaPreferences::is_configurable() {
    return true;
}

AlphaModel::AlphaModel(){
    meta = new AlphaMeta;
}
int AlphaModel::get_size() {
    return contents.size();
}
double AlphaModel::get_value(int n,int row) {
    Q_UNUSED(n);
    return contents.value(row);
}
QVector<double> AlphaModel::get_vector(int n) {
    Q_UNUSED(n);
    return contents;
}
void AlphaModel::set_value(int n ,int row, double value) {
    Q_UNUSED(n);
    contents.replace(row,value);
}
void AlphaModel::insert_value(int n, int row, int count, double value) {
    Q_UNUSED(n);
    contents.insert(row,count,value);
}
void AlphaModel::append_value(int n, double value) {
    Q_UNUSED(n);
    contents.append(value);
}
double AlphaModel::get_raw_value(int n,int row){
    Q_UNUSED(n);
    return raw.value(row);
}
void AlphaModel::set_raw_value(int n ,int row, double value){
    Q_UNUSED(n);
    raw.replace(row,value);
}
void AlphaModel::insert_raw_value(int n, int row, int count, double value) {
    Q_UNUSED(n);
    raw.insert(row,count,value);
}
void AlphaModel::append_raw_value(int n, double value) {
    Q_UNUSED(n);
    raw.append(value);
}
void AlphaModel::set_zero(QVector<double> zero) {
    Q_UNUSED(zero);
}
QVector<double> AlphaModel::get_zero() {
    return QVector<double>();
}
QWidget* AlphaModel::view_get_widget(){
    return NULL;
}
QWidget* AlphaModel::measurement_get_widget(){
    return NULL;
}
bool AlphaModel::measurement_accept_config(VariableModel *m){
    Q_UNUSED(m);
    return true;
}

bool AlphaModel::measurement_is_configurable(){
    return false;
}
void AlphaModel::save_xml(QDomElement root){
    Q_UNUSED(root);
}
void AlphaModel::load_xml(QDomElement root){
    Q_UNUSED(root);
}


AlphaHardware::AlphaHardware(){
    meta = new AlphaMeta;
    QSettings settings;
    arduinofd = init_arduino(settings.value(SETTINGS_ARDUINO_PATH,SETTINGS_ARDUINO_PATH_DEFAULT).toString().toStdString().c_str());

    zero = ANGLEZERO_ALPHA;
    sensitivity = ANGLESENSITIVITY_ALPHA;
    anglemax = ANGLEMAX_ALPHA;
    precision = PRECISION_ALPHA;
    relay_increase = '0';
    relay_decrease = '1';

    std::string filename = settings.value(SETTINGS_ALPHA_PATH,SETTINGS_ALPHA_PATH_DEFAULT).toString().toStdString();
    fp = open(filename.c_str(),O_RDWR);
    if ( fp == -1 ){
        throw std::runtime_error("unable to open alpha device");
    }

}
AlphaHardware::~AlphaHardware(){
    close(fp);
    close(arduinofd);
}
void AlphaHardware::read() {

    if ( ::read(fp, &digits, sizeof(int)) == -1){
        throw std::runtime_error("unable to read from angle device");
    }
    else {
        convert();

        /* WRITE to displays*/
        displays = (int) round(angle*10); //for the first decimal part, loose the rest
        int ret_val = write(fp, &displays, sizeof(int));
        if ( ret_val == -1 || ret_val != sizeof (int)){
            throw std::runtime_error("problem writing to displays");
        }
    }
}
double AlphaHardware::get_value(int n) {
    Q_UNUSED(n);
    return angle;
}
void AlphaHardware::set_value(int n ,double value) {
    Q_UNUSED(n);
    //max
    if (value > anglemax){
        value = anglemax;
    }
    //min
    if (value < -anglemax){
        value = -anglemax;
    }

    read(); //update value

    // if the difference is small do nothing
    if (fabs(value - angle) < precision){
        return ;
    }

    while(fabs(value - angle) >= precision){
        //angle_dest is bigger
        if (value > angle){
            increase(value);
        }
        //angle_dest is smaller
        if (value < angle){
            decrease(value);
        }
        read(); //update value
    }
    read(); //update value
}
double AlphaHardware::get_raw_value(int n){
    Q_UNUSED(n);
    return digits;
}

bool AlphaHardware::isReady(void) {
    return true;
}
bool AlphaHardware::has_set_final() {
    return meta->is_controlable() && false;
}
void AlphaHardware::set_final() {
}
void AlphaHardware::set_zero(QVector<double> zero) {
    Q_UNUSED(zero);
}
void AlphaHardware::increase(double angle_dest){
    set_relay(arduinofd,relay_increase,COMMAND_ON);
    do{
        read(); //update value

    } while((angle_dest - angle) > precision);
    set_relay(arduinofd,relay_increase,COMMAND_OFF);
}
void AlphaHardware::decrease(double angle_dest){
    set_relay(arduinofd,relay_decrease,COMMAND_ON);
    do{
        read(); //update value
    } while((angle - angle_dest) > precision);
    set_relay(arduinofd,relay_decrease,COMMAND_OFF);
}
void AlphaHardware::convert(void){
    angle = ((double)digits - zero) * sensitivity;
    angle = atan(angle)*180/(4*atan(1)); //PI
}



VariableMeta* AlphaFactory::CreateVariableMeta() {
    return new AlphaMeta;
}
VariablePreferences* AlphaFactory::CreateVariableGUI() {
    return new AlphaPreferences;
}
VariableModel* AlphaFactory::CreateVariableModel() {
    return new AlphaModel;
}
VariableHardware* AlphaFactory::CreateVariableHardware(VariableModel *v) {
    Q_UNUSED(v);
    return new AlphaHardware;
}

Q_EXPORT_PLUGIN2(21_alpha, AlphaFactory);

