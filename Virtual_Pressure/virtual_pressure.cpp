#include "virtual_pressure.h"

#define NUM_CHANNELS 8
// the first black box has 8 channels, but only 5 are of real interest.
// to keep consistency of names and numbers, there is the following global variable
bool actives[NUM_CHANNELS]={false,true,true,true,true,true,false,false};

#define SETTINGS_PRESSURE "pressure_channel"
#define SETTINGS_PRESSURE_DEFAULT 1
#define TAG_PRESSURE_CHANNEL "pressure_channel"

bool PressureMeta::is_controlable() {return false;}
bool PressureMeta::has_zero() {return false;}
int PressureMeta::get_num() {return 1;}
QString PressureMeta::get_general_name(){ return "Pressure";}
QString PressureMeta::get_general_name_tr(){
    return QString(QObject::tr("Pressure"));
}
QString PressureMeta::get_name(int n){Q_UNUSED(n);  return "Pressure"; }
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
double PressureMeta::get_lower_bound(int n) {Q_UNUSED(n); return 0;}
double PressureMeta::get_upper_bound(int n) {Q_UNUSED(n); return 0;}
double PressureMeta::get_smaller_step(int n) {Q_UNUSED(n); return 0;}
double PressureMeta::get_default_step(int n) {Q_UNUSED(n); return 0;}
double PressureMeta::get_default_start(int n) {Q_UNUSED(n); return 0;}


PressurePreferences::PressurePreferences() {
}
PressurePreferences::~PressurePreferences(){
}
QWidget* PressurePreferences::get_widget() {
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    QAbstractButton *button;
    group = new QButtonGroup(widget);

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
bool PressurePreferences::is_configurable() {return true;}


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
    group = new QButtonGroup(widget);

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
    channel = dynamic_cast<PressureModel*>(v)->channel;
}
PressureHardware::~PressureHardware(){
}
void PressureHardware::read() { raw = (1.0 * qrand() / RAND_MAX); value = 10 *raw;}
double PressureHardware::get_value(int n) {Q_UNUSED(n); return value;}
double PressureHardware::get_raw_value(int n) {Q_UNUSED(n); return raw;}
void PressureHardware::set_value(int n ,double value) {Q_UNUSED(n); Q_UNUSED(value);  }
bool PressureHardware::isReady(void) {return true;}
bool PressureHardware::has_set_final() {return meta->is_controlable() && false;}
void PressureHardware::set_final() {}
void PressureHardware::set_zero(QVector<double> zero) {Q_UNUSED(zero);}



VariableMeta* PressureFactory::CreateVariableMeta() { return new PressureMeta;}
VariablePreferences* PressureFactory::CreateVariableGUI() { return new PressurePreferences;}
VariableModel* PressureFactory::CreateVariableModel() { return new PressureModel;}
VariableHardware* PressureFactory::CreateVariableHardware(VariableModel *v) { return new PressureHardware(v);}


Q_EXPORT_PLUGIN2(40_pressure, PressureFactory);
