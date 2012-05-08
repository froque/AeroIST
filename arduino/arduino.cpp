#include "arduino.h"


#include <QSettings>

#define ARDUINO_ANALOG_REF 5.0
#define SETTINGS_ARDUINO_PATH "arduino_path"
#define SETTINGS_ARDUINO_PATH_DEFAULT "/dev/arduino_duemilanove"

bool ArduinoMeta::is_controlable() {
    return false;
}
bool ArduinoMeta::has_zero() {
    return false;
}
int ArduinoMeta::get_num() {
    return 0;
}
QString ArduinoMeta::get_general_name(){
    return "Arduino";
}
QString ArduinoMeta::get_general_name_tr(){
    return QString(QObject::tr("Arduino"));
}
QString ArduinoMeta::get_name(int n){
    Q_UNUSED(n);
    return "";
}
QString ArduinoMeta::get_name_tr(int n){
    Q_UNUSED(n);
    return QString(QObject::tr("Arduino"));
}
QString ArduinoMeta::get_units(int n) {
    Q_UNUSED(n);
    return "";
}
double ArduinoMeta::get_lower_bound(int n) {
    Q_UNUSED(n);
    return 0.0;
}
double ArduinoMeta::get_upper_bound(int n) {
    Q_UNUSED(n);
    return 0.0;
}
double ArduinoMeta::get_smaller_step(int n) {
    Q_UNUSED(n);
    return 0.0;
}
double ArduinoMeta::get_default_step(int n) {
    Q_UNUSED(n);
    return 0.0;
}
double ArduinoMeta::get_default_start(int n) {
    Q_UNUSED(n);
    return 0.0;
}

ArduinoPreferences::ArduinoPreferences() {
    meta = new ArduinoMeta();
}
QWidget* ArduinoPreferences::get_widget() {
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;

    layout->addWidget(new QLabel(QObject::tr("Arduino device")),0,0);
    edit_arduino = new QLineEdit;
    edit_arduino->setText(settings.value(SETTINGS_ARDUINO_PATH,SETTINGS_ARDUINO_PATH_DEFAULT).toString());
    layout->addWidget(edit_arduino,0,1);

    widget->setLayout(layout);
    return widget;
}

bool ArduinoPreferences::accept_config() {
    QSettings settings;
    settings.setValue(SETTINGS_ARDUINO_PATH, edit_arduino->text());
    return true;
}
bool ArduinoPreferences::is_configurable() {
    return true;
}


VariableMeta* ArduinoFactory::CreateVariableMeta() {
    return NULL;
}
VariablePreferences* ArduinoFactory::CreateVariableGUI() {
    return new ArduinoPreferences;
}
VariableModel* ArduinoFactory::CreateVariableModel() {
    return NULL;
}
VariableHardware* ArduinoFactory::CreateVariableHardware(VariableModel *v) {
    Q_UNUSED(v);
    return NULL;
}

Q_EXPORT_PLUGIN2(alpha, ArduinoFactory);

