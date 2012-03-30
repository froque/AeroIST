#include "motor.h"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>   /* File control definitions */
#include <iostream>
#include <stdexcept>
#include <cmath>

#include <QSettings>
#include "helper.h"

#define SIMOREG_BUFLEN 8
#define MOTOR_FULLSPEED 16384.0 // 100% according to manual
#define MOTOR_FULLPERCENTAGE 100.0
#define MOTOR_PRECISION 0.1           // in percentage

#define MOTOR_MIN    0
#define MOTOR_MAX    100 //Percentage
#define MOTOR_MIN_STEP 1
#define DEFAULT_MOTOR_STEP   1

#define SETTINGS_MOTOR_PATH "motor_path"
#define SETTINGS_MOTOR_PATH_DEFAULT "/dev/ttyS0"


bool MotorMeta::is_controlable() {
    return true;
}
bool MotorMeta::has_zero() {
    return false;
}
int MotorMeta::get_num() {
    return 1;
}
QString MotorMeta::get_general_name(){
    return "Motor";
}
QString MotorMeta::get_name(int n){
    Q_UNUSED(n);
    return "Motor";
}
QString MotorMeta::get_units(int n) {
    Q_UNUSED(n);
    return "%";
}
double MotorMeta::get_lower_bound(int n) {
    Q_UNUSED(n);
    return MOTOR_MIN;
}
double MotorMeta::get_upper_bound(int n) {
    Q_UNUSED(n);
    return MOTOR_MAX;
}
double MotorMeta::get_smaller_step(int n) {
    Q_UNUSED(n);
    return MOTOR_MIN_STEP;
}
double MotorMeta::get_default_step(int n) {
    Q_UNUSED(n);
    return DEFAULT_MOTOR_STEP;
}
double MotorMeta::get_default_start(int n) {
    Q_UNUSED(n);
    return 0;
}

MotorPreferences::MotorPreferences() {
    meta = new MotorMeta();
}
QWidget* MotorPreferences::get_widget() {
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    layout->addWidget(new QLabel(QObject::tr("Motor device path")),0,0);
    edit_motor = new QLineEdit;
    edit_motor->setText(settings.value(SETTINGS_MOTOR_PATH).toString());
    layout->addWidget(edit_motor,0,1);
    button = new QToolButton;
    QObject::connect(button,SIGNAL(clicked()),this,SLOT(button_slot()));
    layout->addWidget(button,0,2);
    widget->setLayout(layout);
    return widget;
}

bool MotorPreferences::accept_config() {
    QSettings settings;
    settings.setValue(SETTINGS_MOTOR_PATH, edit_motor->text());
    return true;
}
bool MotorPreferences::is_configurable() {
    return true;
}


void MotorPreferences::button_slot(){
    edit_motor->setText(QFileDialog::getOpenFileName(NULL, tr("Choose device"),"/dev", ""));
}

MotorModel::MotorModel(){
    meta = new MotorMeta;
}
int MotorModel::get_size() {
    return contents.size();
}
double MotorModel::get_value(int n,int row) {
    Q_UNUSED(n);
    return contents.value(row);
}
QVector<double> MotorModel::get_vector(int n) {
    Q_UNUSED(n);
    return contents;
}
void MotorModel::set_value(int n ,int row, double value) {
    Q_UNUSED(n);
    contents.replace(row,value);
}
void MotorModel::insert_value(int n, int row, int count, double value) {
    Q_UNUSED(n);
    contents.insert(row,count,value);
}
void MotorModel::append_value(int n, double value) {
    Q_UNUSED(n);
    contents.append(value);
}
double MotorModel::get_raw_value(int n,int row){
    Q_UNUSED(n);
    return raw.value(row);
}
void MotorModel::set_raw_value(int n ,int row, double value){
    Q_UNUSED(n);
    raw.replace(row,value);
}
void MotorModel::insert_raw_value(int n, int row, int count, double value) {
    Q_UNUSED(n);
    raw.insert(row,count,value);
}
void MotorModel::append_raw_value(int n, double value) {
    Q_UNUSED(n);
    raw.append(value);
}
void MotorModel::set_zero(QVector<double> zero) {
    Q_UNUSED(zero);
}
QVector<double> MotorModel::get_zero() {
    return QVector<double>();
}
QWidget* MotorModel::view_get_widget(){
    return NULL;
}
QWidget* MotorModel::measurement_get_widget(){
    return NULL;
}
bool MotorModel::measurement_accept_config(VariableModel *m){
    Q_UNUSED(m);
    return true;
}
bool MotorModel::measurement_is_configurable(){
    return false;
}
void MotorModel::save_xml(QDomElement root){
    Q_UNUSED(root);
}
void MotorModel::load_xml(QDomElement root){
    Q_UNUSED(root);
}


MotorHardware::MotorHardware () {
    meta = new MotorMeta;
    terminal37 = false;
    speed_setpoint = 0;
    speed_actual = 0;

    int n;
    struct termios options;

    QSettings settings;
    std::string filename = settings.value(SETTINGS_MOTOR_PATH).toString().toStdString();
    fd = open(filename.c_str(), O_RDWR );
    if (fd == -1){
        throw std::runtime_error("unable to open serial port");
    }

    /***** setting several options ******/
    tcflush(fd, TCIOFLUSH);
    n = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, n & ~O_NDELAY);
    /* Get the current options for the port... */
    tcgetattr(fd, &options);
    /*  Set the baud rates */
    cfsetospeed(&options, B9600);
    cfsetispeed(&options, B9600);
    /* Enable the receiver and set local mode, even parity and 8 bit word */
    options.c_cflag = (options.c_cflag & ~CSIZE) | CS8;
    options.c_cflag |= CLOCAL;
    options.c_cflag |= CREAD;
    options.c_cflag &= ~(PARENB | PARODD);
    options.c_cflag |= PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_iflag=IGNBRK;
    options.c_iflag &= ~(IXON|IXOFF|IXANY);
    options.c_lflag=0;
    options.c_oflag=0;
    options.c_cc[VTIME]=1;
    options.c_cc[VMIN]=0;
    /*  Set the new options for the port... */
    if (tcsetattr(fd, TCSANOW, &options)!=0){
        throw std::runtime_error("setting attributes failed");
    }

    talk_to_simoreg();
}
MotorHardware::~MotorHardware(){
    close(fd);
}
void MotorHardware::read() {
    talk_to_simoreg();
}
double MotorHardware::get_value(int n) {
    Q_UNUSED(n);
    //fixme: the value to return is percentage for now, but may change in the future
    return speed_actual;
}
void MotorHardware::set_value(int n ,double value) {
    Q_UNUSED(n);
    speed_setpoint = value;
    do{
        talk_to_simoreg();
        Helper::msleep(100);
    } while ( fabs(speed_actual - speed_setpoint) > MOTOR_PRECISION);
}
double MotorHardware::get_raw_value(int n){
    Q_UNUSED(n);
    return speed_actual; // raw is the percentage.
}
bool MotorHardware::isReady(void) {
    talk_to_simoreg();
    return terminal37;
}
bool MotorHardware::has_set_final() {
    return meta->is_controlable() && true;
}
void MotorHardware::set_final() {
    set_value(0,0);
}
void MotorHardware::set_zero(QVector<double> zero) {
    Q_UNUSED(zero);
}

void MotorHardware::convert_velocity(double percentage, unsigned char *nethigh, unsigned char *netlow){
    if (percentage > MOTOR_FULLPERCENTAGE){
        percentage = MOTOR_FULLPERCENTAGE;
    }
    if(percentage < 0){
        percentage = 0.0;
    }
    unsigned int speed = MOTOR_FULLSPEED*percentage/MOTOR_FULLPERCENTAGE;

    *netlow  = speed%256;
    *nethigh = (speed-*netlow)/256;
}

// 0x4000 = 100%
// the drive goes from -200% to 199,9%
double MotorHardware::convert_percentage(unsigned char nethigh, unsigned char netlow){
    int speed = nethigh * 256 + netlow;

    double percentage;
    if (speed <= MOTOR_FULLSPEED *2 ) {
        percentage =  (MOTOR_FULLPERCENTAGE * speed /MOTOR_FULLSPEED);        // 0% - 200%
    } else {
        percentage =  (MOTOR_FULLPERCENTAGE * (speed - MOTOR_FULLSPEED *4) /MOTOR_FULLSPEED); // -200%% - 0%
    }
    return percentage;
}

void MotorHardware::talk_to_simoreg(void){

    unsigned char stx=0x02, lge=0x06, adr=0x0, net1high=0x04, net1low=0x01, net2high=0x00, net2low=0x00, bcc;
    unsigned char buffer[SIMOREG_BUFLEN];
    unsigned char buffer2[SIMOREG_BUFLEN];

    convert_velocity(speed_setpoint, &net2high, &net2low);

    bcc = stx ^ lge ^ adr ^ net1high ^ net1low ^ net2high ^ net2low;
    buffer[0] = stx;
    buffer[1] = lge;
    buffer[2] = adr;
    buffer[3] = net1high;
    buffer[4] = net1low;
    buffer[5] = net2high;
    buffer[6] = net2low;
    buffer[7] = bcc;

    //    printf("SENDING:   stx:%02X lge:%02X adr:%02X net1:%02X%02X net2:%02X%02X bcc:%02X\n",stx,lge,adr,net1high,net1low,net2high,net2low,bcc);

    for(unsigned int k=0;k<sizeof(buffer);k++){
        if ( write(fd, &buffer[k], 1) <1){
            throw std::runtime_error("error on writing to SIMOREG");
        }
    }

    int bytesRead = ::read(fd,buffer2,SIMOREG_BUFLEN);

    if (bytesRead != SIMOREG_BUFLEN) {
        throw std::runtime_error("Error on reading from the SIMOREG serial port");
    }

    bcc=0;
    for( int k=0 ; k<bytesRead-1 ; k++) {
        bcc ^= buffer2[k];
    }

    //    printf("RECEIVING: stx:%02X lge:%02X adr:%02X net1:%02X %02X net2:%02X %02X bcc:%02X\n",buffer2[0],buffer2[1],buffer2[2],buffer2[3],buffer2[4],buffer2[5],buffer2[6],buffer2[7]);

    if (bcc != buffer2[bytesRead-1]) {    
        //        printf("bcc that should be %02X\t\t - bcc that is %02X",bcc,buffer2[bytesRead-1]);
        throw std::runtime_error("error on BCC. Try the green button");
    }

    terminal37 = !(buffer2[4] & 11);
    speed_actual = convert_percentage(buffer2[5], buffer2[6]);
}

VariableMeta* MotorFactory::CreateVariableMeta() {
    return new MotorMeta;
}
VariablePreferences* MotorFactory::CreateVariableGUI() {
    return new MotorPreferences;
}
VariableModel* MotorFactory::CreateVariableModel() {
    return new MotorModel;
}
VariableHardware* MotorFactory::CreateVariableHardware(VariableModel *v) {
    Q_UNUSED(v);
    return new MotorHardware;
}

Q_EXPORT_PLUGIN2(30_motor, MotorFactory);
