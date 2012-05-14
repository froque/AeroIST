#include "angles.h"

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


#define DEFAULT_BETA_STEP   1
#define DEFAULT_BETA_START  0
#define ANGLEZERO_BETA 80751.0
#define ANGLESENSITIVITY_BETA 0.00211000
#define ANGLEMAX_BETA 165.0
#define ANGLEMINSTEP_BETA 0.2
#define PRECISION_BETA 0.08

#define SETTINGS_ANGLES_PATH "angles_path"
#define SETTINGS_ANGLES_PATH_DEFAULT "/dev/SSI_converter"

#define ARDUINO_ANALOG_REF 5.0
#define SETTINGS_ARDUINO_PATH "arduino_path"
#define SETTINGS_ARDUINO_PATH_DEFAULT "/dev/arduino_duemilanove"
/*
        The purpose of this function is to convert a reflected binary
        Gray code number to a binary number. Source: wikipedia
*/
unsigned int grayToBinary(unsigned int num)
{
    unsigned int numBits = 8 * sizeof(num);
    unsigned int shift;
    for (shift = 1; shift < numBits; shift *= 2)
    {
        num ^= num >> shift;
    }
    return num;
}


bool AnglesMeta::is_controlable() {
    return true;
//    return false;
}
bool AnglesMeta::has_zero() {
    return false;
}
int AnglesMeta::get_num() {
    return 2;
}
QString AnglesMeta::get_general_name(){
    return "Angles";
}
QString AnglesMeta::get_general_name_tr(){
    return QString(QObject::tr("Angles"));
}
QString AnglesMeta::get_name(int n){
    switch(n){
    case 0: return "Alpha"; break;
    case 1: return "Beta"; break;
    default: return "";
    }
}
QString AnglesMeta::get_name_tr(int n){
    switch(n){
    case 0: return QString(QObject::tr("Alpha")); break;
    case 1: return QString(QObject::tr("Beta")); break;
    default: return "";
    }
}
QString AnglesMeta::get_units(int n) {
    Q_UNUSED(n);
    return QString(QObject::trUtf8("º"));
}
QString AnglesMeta::get_raw_units(int n) {
    Q_UNUSED(n);
    return QString(QObject::trUtf8("step"));
}
double AnglesMeta::get_lower_bound(int n) {
    switch(n){
    case 0: return -ANGLEMAX_ALPHA; break;
    case 1: return -ANGLEMAX_BETA; break;
    default: return 0;
    }
}
double AnglesMeta::get_upper_bound(int n) {
    switch(n){
    case 0: return ANGLEMAX_ALPHA; break;
    case 1: return ANGLEMAX_BETA; break;
    default: return 0;
    }
}
double AnglesMeta::get_smaller_step(int n) {
    switch(n){
    case 0: return ANGLEMINSTEP_ALPHA; break;
    case 1: return ANGLEMINSTEP_BETA; break;
    default: return 0;
    }
}
double AnglesMeta::get_default_step(int n) {
    switch(n){
    case 0: return DEFAULT_ALPHA_STEP; break;
    case 1: return DEFAULT_BETA_STEP; break;
    default: return 0;
    }
}
double AnglesMeta::get_default_start(int n) {
    switch(n){
    case 0: return DEFAULT_ALPHA_START; break;
    case 1: return DEFAULT_BETA_START; break;
    default: return 0;
    }
}


AnglesPreferences::AnglesPreferences() {
    meta = new AnglesMeta();
}
AnglesPreferences::~AnglesPreferences(){
    delete meta;
}
QWidget* AnglesPreferences::get_widget() {
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    layout->addWidget(new QLabel(QObject::tr("Angles device")),0,0);
    edit_angles = new QLineEdit;
    edit_angles->setText(settings.value(SETTINGS_ANGLES_PATH,SETTINGS_ANGLES_PATH_DEFAULT).toString());
    layout->addWidget(edit_angles,0,1);
    widget->setLayout(layout);
    return widget;
}

bool AnglesPreferences::accept_config() {
    QSettings settings;
    settings.setValue(SETTINGS_ANGLES_PATH, edit_angles->text());
    return true;
}
bool AnglesPreferences::is_configurable() {
    return true;
}

AnglesModel::AnglesModel(){
    meta = new AnglesMeta;
}
AnglesModel::~AnglesModel(){
    delete meta;
}
int AnglesModel::get_size() {
    return contents[0].size();
}
double AnglesModel::get_value(int n,int row) {
    return contents[n].value(row);
}
QVector<double> AnglesModel::get_vector(int n) {
    return contents[n];
}
void AnglesModel::set_value(int n ,int row, double value) {
    contents[n].replace(row,value);
}
void AnglesModel::insert_value(int n, int row, int count, double value) {
    contents[n].insert(row,count,value);
}
void AnglesModel::append_value(int n, double value) {
    contents[n].append(value);
}
double AnglesModel::get_raw_value(int n,int row){
    return raw[n].value(row);
}
void AnglesModel::set_raw_value(int n ,int row, double value){
    raw[n].replace(row,value);
}
void AnglesModel::insert_raw_value(int n, int row, int count, double value) {
    raw[n].insert(row,count,value);
}
void AnglesModel::append_raw_value(int n, double value) {
    raw[n].append(value);
}
void AnglesModel::set_zero(QVector<double> zero) {
    Q_UNUSED(zero);
}
QVector<double> AnglesModel::get_zero() {
    return QVector<double>();
}
QWidget* AnglesModel::view_get_widget(){
    return NULL;
}
QWidget* AnglesModel::measurement_get_widget(){
    return NULL;
}
bool AnglesModel::measurement_accept_config(VariableModel *m){
    Q_UNUSED(m);
    return true;
}

bool AnglesModel::measurement_is_configurable(){
    return false;
}
void AnglesModel::save_xml(QDomElement root){
    Q_UNUSED(root);
}
void AnglesModel::load_xml(QDomElement root){
    Q_UNUSED(root);
}


AnglesHardware::AnglesHardware(){
    meta = new AnglesMeta;
    QSettings settings;
    arduinofd = init_arduino(settings.value(SETTINGS_ARDUINO_PATH,SETTINGS_ARDUINO_PATH_DEFAULT).toString().toStdString().c_str());

    zero[0] = ANGLEZERO_ALPHA;
    zero[1] = ANGLEZERO_BETA;
    sensitivity[0] = ANGLESENSITIVITY_ALPHA;
    sensitivity[1] = ANGLESENSITIVITY_BETA;
    anglemax[0] = ANGLEMAX_ALPHA;
    anglemax[1] = ANGLEMAX_BETA;
    precision[0] = PRECISION_ALPHA;
    precision[1] = PRECISION_BETA;
    relay_increase[0] = '0';
    relay_increase[1] = '2';
    relay_decrease[0] = '1';
    relay_decrease[1] = '3';

    port = new SerialPort;
    port->setPort(settings.value(SETTINGS_ANGLES_PATH,SETTINGS_ANGLES_PATH_DEFAULT).toString());
    port->setRate(SerialPort::Rate115200);
    port->setDataBits(SerialPort::Data8);
    port->setParity(SerialPort::NoParity);
    port->setStopBits(SerialPort::OneStop);
    if( port->open(QIODevice::ReadWrite | QIODevice::Unbuffered) != true){
        throw std::runtime_error("unable to open angles device");
        return;
    }

    QString command = "$0L1251";                // set 24 bits +1 bit PBF for encoder 1, parity on
    command.append('\r');
    int status = port->write(command.toAscii());
    if (status != command.size()){
        qDebug() << "not a good write to encoder 1";
    }
    if (port->waitForReadyRead(20) == false){
        qDebug() << "error on waiting from encoder 1 " << status;
        return ;
    }
    QString output = port->readAll();
    if (output.contains("NACK")){
        qDebug() << "failed response from SSI to USB from encoder 1";
    }
    command = "$0L2251";                        // set 24 bits +1 bit PBF for encoder 2, parity on
    command.append('\r');
    status = port->write(command.toAscii());
    if (status != command.size()){
        qDebug() << "not a good write to encoder 2";
    }
    if (port->waitForReadyRead(20) == false){
        qDebug() << "error on waiting from encoder 2";
        return ;
    }
    output = port->readAll();
    if (output.contains("NACK")){
        qDebug() << "failed response from SSI to USB from encoder 2";
    }
}
AnglesHardware::~AnglesHardware(){
    port->close();
    close(arduinofd);
    delete meta;
}
void AnglesHardware::read() {
    QString command;
    QString output;
    int tries = 0;
    bool ok1 = false, ok2 = false;
    unsigned gray1 = 0,gray2 = 0;
    while ((ok1 && ok2) == false){
        // give up after 5 tries;
        if(tries > 5){
            throw std::runtime_error("Problem in reading from SSI converter");
        }

        command = "$0R1";
        command.append('\r');
        port->write(command.toAscii());
        if (port->waitForReadyRead(50) == false){
            tries++;
            continue;
        }
        output = port->readAll();
        if (output.contains("NACK")){
            tries++;
            continue;
        }

        gray1 = output.mid(4,10).toUInt(&ok1);            // length 10 for 25 bits

        command = "$0R2";
        command.append('\r');
        port->write(command.toAscii());
        if (port->waitForReadyRead(50) == false){
            tries++;
            continue;
        }
        output = port->readAll();
        if (output.contains("NACK")){
            tries++;
            continue;
        }

        gray2 = output.mid(4,10).toUInt(&ok2);            // length 10 for 25 bits

        if ((ok1 != true) || (ok2 != true)){
            tries++;
            continue;
        }
    }
    unsigned int value1 = grayToBinary(gray1);
    unsigned int value2 = grayToBinary(gray2);
    digits[0] = static_cast<int>(value1);
    digits[1] = static_cast<int>(value2);
    convert_alpha();
    convert_beta();
//    qDebug() <<  "gray: " << gray1 << " - " << value1 << gray2 << " - " << value2 << " :end" << ok1 << ok2 << output;

}
double AnglesHardware::get_value(int n) {
    return angle[n];
}
void AnglesHardware::set_value(int n ,double value) {
    //max
    if (value > anglemax[n]){
        value = anglemax[n];
    }
    //min
    if (value < -anglemax[n]){
        value = -anglemax[n];
    }

    read(); //update value

    // if the difference is small do nothing
    if (fabs(value - angle[n]) < precision[n]){
        return ;
    }

    while(fabs(value - angle[n]) >= precision[n]){
        //angle_dest is bigger
        if (value > angle[n]){
            increase(n,value);
        }
        //angle_dest is smaller
        if (value < angle[n]){
            decrease(n,value);
        }
        read(); //update value
    }
    read(); //update value
}
double AnglesHardware::get_raw_value(int n){
    return digits[n];
}

bool AnglesHardware::isReady(void) {
    return true;
}
bool AnglesHardware::has_set_final() {
    return meta->is_controlable() && false;
}
void AnglesHardware::set_final() {
}
void AnglesHardware::set_zero(QVector<double> zero) {
    Q_UNUSED(zero);
}
void AnglesHardware::increase(int n, double angle_dest){
    set_relay(arduinofd,relay_increase[n],COMMAND_ON);
    do{
        read(); //update value

    } while((angle_dest - angle[n]) > precision[n]);
    set_relay(arduinofd,relay_increase[n],COMMAND_OFF);
}
void AnglesHardware::decrease(int n, double angle_dest){
    set_relay(arduinofd,relay_decrease[n],COMMAND_ON);
    do{
        read(); //update value
    } while((angle[n] - angle_dest) > precision[n]);
    set_relay(arduinofd,relay_decrease[n],COMMAND_OFF);
}
void AnglesHardware::convert_alpha(void){
    angle[0] = ((double)digits[0] - zero[0]) * sensitivity[0];
    angle[0] = atan(angle[0])*180/(4*atan(1)); //PI
}
void AnglesHardware::convert_beta(void){
    angle[1] = ((double)digits[1] - zero[1]) * sensitivity[1];
}



VariableMeta* AnglesFactory::CreateVariableMeta() {
    return new AnglesMeta;
}
VariablePreferences* AnglesFactory::CreateVariableGUI() {
    return new AnglesPreferences;
}
VariableModel* AnglesFactory::CreateVariableModel() {
    return new AnglesModel;
}
VariableHardware* AnglesFactory::CreateVariableHardware(VariableModel *v) {
    Q_UNUSED(v);
    return new AnglesHardware;
}

Q_EXPORT_PLUGIN2(20_angles, AnglesFactory);

