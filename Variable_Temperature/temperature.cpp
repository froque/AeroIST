#include "temperature.h"
#include "arduino-serial.h"

#include <stdexcept>
#include <QSettings>

#define TEMPERATURE_SENSITIVITY 100.0
// OP-AMP Gain = 1 + R5/R4 = 1 + 158000/11500 = 14,74
#define OPAMP_GAIN 14.74

#define ARDUINO_ANALOG_REF 5.0
#define SETTINGS_ARDUINO_PATH "arduino_path"
#define SETTINGS_ARDUINO_PATH_DEFAULT "/dev/arduino_duemilanove"
#define SETTINGS_TEMP_SENSOR "temperature_sensor"
#define SETTINGS_TEMP_SENSOR_DEFAULT 1

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
QString TemperatureMeta::get_general_name_tr(){
    return QString(QObject::tr("Temperature"));
}
QString TemperatureMeta::get_name(int n){
    Q_UNUSED(n);
    return "Temperature";
}
QString TemperatureMeta::get_name_tr(int n){
    Q_UNUSED(n);
    return QString(QObject::tr("Temperature"));
}
QString TemperatureMeta::get_units(int n) {
    Q_UNUSED(n);
    return QString(QObject::trUtf8("°C"));
}
QString TemperatureMeta::get_raw_units(int n) {
    Q_UNUSED(n);
    return QString(QObject::tr("step"));
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
TemperaturePreferences::~TemperaturePreferences(){
    delete meta;
}
QWidget* TemperaturePreferences::get_widget() {
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;

    group = new QButtonGroup;
    QAbstractButton *button;

    button = new QRadioButton(QString(QObject::tr("Onboard")));
    group->addButton(button,0);
    layout->addWidget(button,0,0);
    button = new QRadioButton(QString(QObject::tr("APC external")));
    group->addButton(button,1);
    layout->addWidget(button,0,1);

    button = group->button(settings.value(SETTINGS_TEMP_SENSOR,SETTINGS_TEMP_SENSOR_DEFAULT).toInt());
    if(button != 0){
        button->setChecked(true);
    } else {
        group->buttons().first()->setChecked(true);
    }

    widget->setLayout(layout);
    return widget;
}
bool TemperaturePreferences::accept_config() {
    QSettings settings;
    settings.setValue(SETTINGS_TEMP_SENSOR,group->checkedId());
    return true;
}
bool TemperaturePreferences::is_configurable() {
    return true;
}


TemperatureModel::TemperatureModel(){
    meta = new TemperatureMeta;
}
TemperatureModel::~TemperatureModel(){
    delete meta;
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
    arduinofd = serialport_init(settings.value(SETTINGS_ARDUINO_PATH,   SETTINGS_ARDUINO_PATH_DEFAULT).toString().toStdString().c_str(),SERIALRATE);
    sensor = settings.value(SETTINGS_TEMP_SENSOR,SETTINGS_TEMP_SENSOR_DEFAULT).toInt();
}

TemperatureHardware::~TemperatureHardware() {
    close(arduinofd);
    delete meta;
}
void TemperatureHardware::read() {
    char buffer_read[256]="", buffer_aux[256];
    char buffer[] = "$A0xxxx\n";               // read from analog 0
    bool sucess=false;
    int tries = 0;

    if (sensor == 0){
        buffer[2] = '0';
    } else {
        buffer[2] = '1';
    }

    // response form: $CIXXXX\n
    // $ is a start byte
    // \n is a stop byte

    while (sucess == false){
        serialport_flush(arduinofd);
        if( serialport_write(arduinofd, buffer) == -1){
            throw std::runtime_error("Problem in reading from Temperature");
        }

        serialport_read_until(arduinofd, buffer_read, '\n');

        if (strncmp(buffer,buffer_read,3)==0){
            strncpy(buffer_aux,buffer_read + 3,4);
            temp_raw = atoi(buffer_aux);
            sucess=true;
        } else {
            // give up after 5 tries;
            tries++;
            if(tries > 5){
                throw std::runtime_error("Problem in reading from Temperature");
            }
        }
    }
    // 5.0 V
    // 10 bits = 1024
    // 100 ºC/V = 0,01 V/ºC
    if (sensor == 0){
        temp = temp_raw * ARDUINO_ANALOG_REF * TEMPERATURE_SENSITIVITY /1024.0;
    } else {
        // OP-AMP Gain = 1 + R5/R4 = 1 + 158000/11500 = 14,74
        temp = temp_raw * ARDUINO_ANALOG_REF * TEMPERATURE_SENSITIVITY / OPAMP_GAIN /1024.0;
    }
}


double TemperatureHardware::get_value(int n) {
    Q_UNUSED(n);
    return temp;
}
void TemperatureHardware::set_value(int n ,double value) {
    Q_UNUSED(n);
    Q_UNUSED(value);
}
double TemperatureHardware::get_raw_value(int n){
    Q_UNUSED(n);
    return temp_raw;
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

Q_EXPORT_PLUGIN2(50_temperature, TemperatureFactory);

