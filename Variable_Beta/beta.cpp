#include "beta.h"

#include <fcntl.h>
#include <stdexcept>
#include <QSettings>

#include "botoneira.h"

#define DEFAULT_BETA_STEP   5
#define DEFAULT_BETA_START  0
#define ANGLEZERO_BETA 80751.0
#define ANGLESENSITIVITY_BETA 0.00211000
#define ANGLEMAX_BETA 165.0
#define ANGLEMINSTEP_BETA 0.2
#define PRECISION_BETA 0.05

#define SETTINGS_BETA_PATH "beta_path"
#define SETTINGS_BETA_PATH_DEFAULT "/dev/angle_beta"


#define COMMAND_ON 1
#define COMMAND_OFF 0

#define ARDUINO_ANALOG_REF 5.0
#define SETTINGS_ARDUINO_PATH "arduino_path"
#define SETTINGS_ARDUINO_PATH_DEFAULT "/dev/arduino_duemilanove"

#define SETTINGS_BETA_PATH "beta_path"
#define SETTINGS_BETA_PATH_DEFAULT "/dev/angle_beta"

bool BetaMeta::is_controlable() {
    return true;
}
bool BetaMeta::has_zero() {
    return false;
}
int BetaMeta::get_num() {
    return 1;
}
QString BetaMeta::get_general_name(){
    return "Beta";
}
QString BetaMeta::get_general_name_tr(){
    return QString(QObject::tr("Beta"));
}
QString BetaMeta::get_name(int n){
    Q_UNUSED(n);
    return "Beta";
}
QString BetaMeta::get_name_tr(int n){
    Q_UNUSED(n);
    return QString(QObject::tr("Beta"));
}
QString BetaMeta::get_units(int n) {
    Q_UNUSED(n);
    return QString(QObject::trUtf8("º"));
}
double BetaMeta::get_lower_bound(int n) {
    Q_UNUSED(n);
    return -ANGLEMAX_BETA;
}
double BetaMeta::get_upper_bound(int n) {
    Q_UNUSED(n);
    return ANGLEMAX_BETA;
}
double BetaMeta::get_smaller_step(int n) {
    Q_UNUSED(n);
    return ANGLEMINSTEP_BETA;
}
double BetaMeta::get_default_step(int n) {
    Q_UNUSED(n);
    return DEFAULT_BETA_STEP;
}
double BetaMeta::get_default_start(int n) {
    Q_UNUSED(n);
    return DEFAULT_BETA_START;
}


BetaPreferences::BetaPreferences() {
    meta = new BetaMeta();
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
bool BetaPreferences::is_configurable() {
    return true;
}

BetaModel::BetaModel(){
    meta = new BetaMeta;
}
int BetaModel::get_size() {
    return contents.size();
}
double BetaModel::get_value(int n,int row) {
    Q_UNUSED(n);
    return contents.value(row);
}
QVector<double> BetaModel::get_vector(int n) {
    Q_UNUSED(n);
    return contents;
}
void BetaModel::set_value(int n ,int row, double value) {
    Q_UNUSED(n);
    contents.replace(row,value);
}
void BetaModel::insert_value(int n, int row, int count, double value) {
    Q_UNUSED(n);
    contents.insert(row,count,value);
}
void BetaModel::append_value(int n, double value) {
    Q_UNUSED(n);
    contents.append(value);
}double BetaModel::get_raw_value(int n,int row){
    Q_UNUSED(n);
    return raw.value(row);
}
void BetaModel::set_raw_value(int n ,int row, double value){
    Q_UNUSED(n);
    raw.replace(row,value);
}
void BetaModel::insert_raw_value(int n, int row, int count, double value) {
    Q_UNUSED(n);
    raw.insert(row,count,value);
}
void BetaModel::append_raw_value(int n, double value) {
    Q_UNUSED(n);
    raw.append(value);
}
void BetaModel::set_zero(QVector<double> zero) {
    Q_UNUSED(zero);
}
QVector<double> BetaModel::get_zero() {
    return QVector<double>();
}
QWidget* BetaModel::view_get_widget(){
    return NULL;
}
QWidget* BetaModel::measurement_get_widget(){
    return NULL;
}
bool BetaModel::measurement_accept_config(VariableModel *m){
    Q_UNUSED(m);
    return true;
}

bool BetaModel::measurement_is_configurable(){
    return false;
}
void BetaModel::save_xml(QDomElement root){
    Q_UNUSED(root);
}
void BetaModel::load_xml(QDomElement root){
    Q_UNUSED(root);
}


BetaHardware::BetaHardware(){
    meta = new BetaMeta;
    QSettings settings;
    arduinofd = init_arduino(settings.value(SETTINGS_ARDUINO_PATH,SETTINGS_ARDUINO_PATH_DEFAULT).toString().toStdString().c_str());

    zero = ANGLEZERO_BETA;
    sensitivity = ANGLESENSITIVITY_BETA;
    anglemax = ANGLEMAX_BETA;
    precision = PRECISION_BETA;
    relay_increase = '2';
    relay_decrease = '3';

    std::string filename = settings.value(SETTINGS_BETA_PATH,SETTINGS_BETA_PATH_DEFAULT).toString().toStdString();
    fp = open(filename.c_str(),O_RDWR);
    if ( fp == -1 ){
        throw std::runtime_error("unable to open beta device");
    }
}
BetaHardware::~BetaHardware(){
    close(fp);
    close(arduinofd);
}
void BetaHardware::read() {

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
double BetaHardware::get_value(int n) {
    Q_UNUSED(n);
    return angle;
}
void BetaHardware::set_value(int n ,double value) {
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
double BetaHardware::get_raw_value(int n){
    Q_UNUSED(n);
    return digits;
}
bool BetaHardware::isReady(void) {
    return true;
}
bool BetaHardware::has_set_final() {
    return meta->is_controlable() && false;
}
void BetaHardware::set_final() {
}
void BetaHardware::set_zero(QVector<double> zero) {
    Q_UNUSED(zero);
}
void BetaHardware::increase(double angle_dest){
    set_relay(arduinofd,relay_increase,COMMAND_ON);
    do{
        read(); //update value

    } while((angle_dest - angle) > precision);
    set_relay(arduinofd,relay_increase,COMMAND_OFF);
}
void BetaHardware::decrease(double angle_dest){
    set_relay(arduinofd,relay_decrease,COMMAND_ON);
    do{
        read(); //update value
    } while((angle - angle_dest) > precision);
    set_relay(arduinofd,relay_decrease,COMMAND_OFF);
}
void BetaHardware::convert(void){
    angle = ((double)digits - zero) * sensitivity;
}



VariableMeta* BetaFactory::CreateVariableMeta() {
    return new BetaMeta;
}
VariablePreferences* BetaFactory::CreateVariableGUI() {
    return new BetaPreferences;
}
VariableModel* BetaFactory::CreateVariableModel() {
    return new BetaModel;
}
VariableHardware* BetaFactory::CreateVariableHardware(VariableModel *v) {
    Q_UNUSED(v);
    return new BetaHardware;
}

Q_EXPORT_PLUGIN2(22_beta, BetaFactory);

