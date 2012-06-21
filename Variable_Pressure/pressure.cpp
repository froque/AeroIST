#include "pressure.h"
#include "arduino-serial.h"

#include <stdexcept>
#include <QSettings>

#define PRESSURE_SENSITIVITY 3.98
#define MMH2O_TO_PASCAL 9.80665

#define ARDUINO_ANALOG_REF 5.0
#define SETTINGS_ARDUINO_PATH "arduino_path"
#define SETTINGS_ARDUINO_PATH_DEFAULT "/dev/arduino_duemilanove"

#define NUM_CHANNELS 8
// the first black box has 8 channels, but only 5 are of real interest.
// to keep consistency of names and numbers, there is the following global variable
bool actives[NUM_CHANNELS]={false,true,true,true,true,true,false,false};

#define SETTINGS_PRESSURE "pressure_channel"
#define SETTINGS_PRESSURE_DEFAULT 1
#define TAG_PRESSURE_CHANNEL "pressure_channel"

bool PressureMeta::is_controlable() {
    return false;
}
bool PressureMeta::has_zero() {
    return false;
}
int PressureMeta::get_num() {
    return 1;
}
QString PressureMeta::get_general_name(){
    return "Pressure";
}
QString PressureMeta::get_general_name_tr(){
    return QString(QObject::tr("Pressure"));
}
QString PressureMeta::get_name(int n){
    Q_UNUSED(n);
    return "Pressure";
}
QString PressureMeta::get_name_tr(int n){
    Q_UNUSED(n);
    return QString(QObject::tr("Pressure"));
}
QString PressureMeta::get_units(int n) {
    Q_UNUSED(n);
    return QString(QObject::tr("mmH20"));
}
QString PressureMeta::get_raw_units(int n) {
    Q_UNUSED(n);
    return QString(QObject::tr("step"));
}
double PressureMeta::get_lower_bound(int n) {
    Q_UNUSED(n);
    return 0;
}
double PressureMeta::get_upper_bound(int n) {
    Q_UNUSED(n);
    return 0;
}
double PressureMeta::get_smaller_step(int n) {
    Q_UNUSED(n);
    return 0;
}
double PressureMeta::get_default_step(int n) {
    Q_UNUSED(n);
    return 0;
}
double PressureMeta::get_default_start(int n) {
    Q_UNUSED(n);
    return 0;
}


PressurePreferences::PressurePreferences() {
}
PressurePreferences::~PressurePreferences() {
}
QWidget* PressurePreferences::get_widget() {
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    QAbstractButton *button;
    group = new QButtonGroup;

    // count the number of active channes
    int num_actives=0;
    for (int k=0; k< NUM_CHANNELS; k++){
        if(actives[k] == true){
            num_actives++;
        }
    }

    // to divide the number of active channels in 2 for display in a grid
    int half = (num_actives%2==0)? num_actives/2 : (num_actives+1)/2;

    // beware of start indices!
    for (int k=1, j=1; k<= NUM_CHANNELS; k++){
        if(actives[k-1] == true){
            button = new QRadioButton(QString(QObject::tr("channel %1")).arg(k));
            group->addButton(button,k);
            layout->addWidget(button,(j-1)%half,((j<=half)? 0:1));
            j++;
        }
    }

    button = group->button(settings.value(SETTINGS_PRESSURE,SETTINGS_PRESSURE_DEFAULT).toInt());
    if(button != 0){
        button->setChecked(true);
    } else {
        group->buttons().first()->setChecked(true);
    }

    widget->setLayout(layout);
    return widget;
}
bool PressurePreferences::accept_config() {
    QSettings settings;
    settings.setValue(SETTINGS_PRESSURE,group->checkedId());
    return true;
}
bool PressurePreferences::is_configurable() {
    return true;
}


PressureModel::PressureModel(){
}
PressureModel::~PressureModel(){
}
QWidget* PressureModel::view_get_widget(){
    QWidget *widget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(new QLabel(QObject::tr("Channel")));
    layout->addWidget(new QLabel(QString::number(channel)));
    widget->setLayout(layout);
    return widget;
}
QWidget* PressureModel::measurement_get_widget(){
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    QAbstractButton *button;
    group = new QButtonGroup;

    // count the number of active channes
    int num_actives=0;
    for (int k=0; k< NUM_CHANNELS; k++){
        if(actives[k] == true){
            num_actives++;
        }
    }

    // to divide the number of active channels in 2 for display in a grid
    int half = (num_actives%2==0)? num_actives/2 : (num_actives+1)/2;

    // beware of start indices!
    for (int k=1, j=1; k<= NUM_CHANNELS; k++){
        if(actives[k-1] == true){
            button = new QRadioButton(QString(QObject::tr("channel %1")).arg(k));
            group->addButton(button,k);
            layout->addWidget(button,(j-1)%half,((j<=half)? 0:1));
            j++;
        }
    }

    button = group->button(settings.value(SETTINGS_PRESSURE,SETTINGS_PRESSURE_DEFAULT).toInt());
    if(button != 0){
        button->setChecked(true);
    } else {
        group->buttons().first()->setChecked(true);
    }
    widget->setLayout(layout);
    return widget;
}
bool PressureModel::measurement_accept_config(VariableModel *m){
    Q_UNUSED(m);
    channel = group->checkedId();
    // it really does not make sense to check unless it is included in the reference
/*
    if (m != NULL){
        if (channel != dynamic_cast<PressureModel*>(m)->channel){
            QMessageBox message;
            message.setText("The channels are different.");
            message.exec();
        }
    }
*/
    return true;
}
bool PressureModel::measurement_is_configurable(){
    return true;
}
void PressureModel::save_xml(QDomElement root){
    QDomElement channel_element = root.ownerDocument().createElement(TAG_PRESSURE_CHANNEL);
    channel_element.appendChild(root.ownerDocument().createTextNode(QString::number(this->channel)));
    root.appendChild(channel_element);
}
void PressureModel::load_xml(QDomElement root){
    QDomNodeList nodelist = root.childNodes();
    QDomNode node;
    QDomElement element;
    for (int k=0; k< nodelist.count();k++){
        node = nodelist.at(k);
        element = node.toElement();
        if (element.tagName() == TAG_PRESSURE_CHANNEL){
            this->channel = element.text().toInt();
            continue;
        }
    }
}


PressureHardware::PressureHardware(VariableModel* v) {
    press=0;
    press_raw=0;
    channel = dynamic_cast<PressureModel*>(v)->channel;
    QSettings settings;
    arduinofd = serialport_init(settings.value(SETTINGS_ARDUINO_PATH,SETTINGS_ARDUINO_PATH_DEFAULT).toString().toStdString().c_str(),SERIALRATE);

    int tries = 0;
    bool sucess=false;
    char buffer_read[256]="";
    char buffer[20];
    // on the arduino channel starts at 0. But for the user it starts at 1
    sprintf(buffer,"$CS%dxxx\n",channel-1);

    while (sucess == false){
        serialport_flush(arduinofd);
        if( serialport_write(arduinofd, buffer) == -1){
            throw std::runtime_error("Problem in reading from Pressure");
        }

        serialport_read_until(arduinofd, buffer_read, '\n');

        if (strncmp(buffer,buffer_read,3)==0){
            sucess=true;
        } else {
            // give up after 5 tries;
            tries++;
            if(tries > 5){
                throw std::runtime_error("Problem in Pressure");
            }
        }
    }

    strcpy(buffer,"$CExxxx\n");

    tries = 0;
    sucess =  false;
    while (sucess == false){
        serialport_flush(arduinofd);
        if( serialport_write(arduinofd, buffer) == -1){
            throw std::runtime_error("Problem in reading from Pressure");
        }

        serialport_read_until(arduinofd, buffer_read, '\n');

        if (strncmp(buffer,buffer_read,3)==0){
            sucess=true;
        } else {
            // give up after 5 tries;
            tries++;
            if(tries > 5){
                throw std::runtime_error("Problem in Pressure");
            }
        }
    }
}
PressureHardware::~PressureHardware() {
  serialport_write(arduinofd, "$CDxxxx\n");
  close(arduinofd);
}
void PressureHardware::read() {
    char buffer_read[256]="", buffer_aux[256];
    char buffer[] = "$A5xxxx\n";               // read from analog 5
    bool sucess=false;
    int tries = 0;
    // response form: $CIXXXX\n
    // $ is a start byte
    // \n is a stop byte

    while (sucess == false){
        serialport_flush(arduinofd);
        if( serialport_write(arduinofd, buffer) == -1){
            throw std::runtime_error("Problem in reading from Pressure");
        }

        serialport_read_until(arduinofd, buffer_read, '\n');

        if (strncmp(buffer,buffer_read,3)==0){
            strncpy(buffer_aux,buffer_read + 3,4);
            press_raw = atoi(buffer_aux);
            sucess=true;
        } else {
            // give up after 5 tries;
            tries++;
            if(tries > 5){
                throw std::runtime_error("Problem in reading from Pressure");
            }
        }
    }
    // 5.0 V
    // 10 bits = 1024
    // 19.9 mm H20 = 5 V => 3.98 mm H20/V
    // 1 mm H20 = 9.80665 Pa
//    press = press_raw * ARDUINO_ANALOG_REF/1024.0 * PRESSURE_SENSITIVITY * MMH2O_TO_PASCAL ;
    press = press_raw * ARDUINO_ANALOG_REF/1024.0 * PRESSURE_SENSITIVITY ;
}
double PressureHardware::get_value(int n) {
    Q_UNUSED(n);
    return press;
}
void PressureHardware::set_value(int n ,double value) {
    Q_UNUSED(n);
    Q_UNUSED(value);
}
double PressureHardware::get_raw_value(int n){
    Q_UNUSED(n);
    return press_raw;
}
bool PressureHardware::isReady(void) {
    return true;
}
bool PressureHardware::has_set_final() {
    return meta->is_controlable() && false;
}
void PressureHardware::set_final() {
}
void PressureHardware::set_zero(QVector<double> zero) {
    Q_UNUSED(zero);
}



VariableMeta* PressureFactory::CreateVariableMeta() {
    return new PressureMeta;
}
VariablePreferences* PressureFactory::CreateVariableGUI() {
    return new PressurePreferences;
}
VariableModel* PressureFactory::CreateVariableModel() {
    return new PressureModel;
}
VariableHardware* PressureFactory::CreateVariableHardware(VariableModel *v) {
    return new PressureHardware(v);
}


Q_EXPORT_PLUGIN2(40_pressure, PressureFactory);

