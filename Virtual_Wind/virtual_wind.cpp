#include "virtual_wind.h"

#define NUM_CHANNELS 8
// the first black box has 8 channels, but only 5 are of real interest.
// to keep consistency of names and numbers, there is the following global variable
bool actives[NUM_CHANNELS]={false,true,true,true,true,true,false,false};

#define SETTINGS_WIND "wind_channel"
#define SETTINGS_WIND_DEFAULT 1
#define TAG_WIND_CHANNEL "wind_channel"

bool WindMeta::is_controlable() {return false;}
bool WindMeta::has_zero() {return false;}
int WindMeta::get_num() {return 1;}
QString WindMeta::get_general_name(){ return "Wind";}
QString WindMeta::get_general_name_tr(){
    return QString(QObject::tr("Wind"));
}
QString WindMeta::get_name(int n){Q_UNUSED(n);  return "Wind"; }
QString WindMeta::get_name_tr(int n){
    Q_UNUSED(n);
    return QString(QObject::tr("Wind"));
}
QString WindMeta::get_units(int n) {
    Q_UNUSED(n);
    return QString(QObject::tr("mmH20"));
}
QString WindMeta::get_raw_units(int n) {
    Q_UNUSED(n);
    return QString(QObject::tr("step"));
}
double WindMeta::get_lower_bound(int n) {Q_UNUSED(n); return 0;}
double WindMeta::get_upper_bound(int n) {Q_UNUSED(n); return 0;}
double WindMeta::get_smaller_step(int n) {Q_UNUSED(n); return 0;}
double WindMeta::get_default_step(int n) {Q_UNUSED(n); return 0;}
double WindMeta::get_default_start(int n) {Q_UNUSED(n); return 0;}


WindPreferences::WindPreferences() {
}
WindPreferences::~WindPreferences(){
}
QWidget* WindPreferences::get_widget() {
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

    button = group->button(settings.value(SETTINGS_WIND,SETTINGS_WIND_DEFAULT).toInt());
    if(button != 0){
        button->setChecked(true);
    } else {
        group->buttons().first()->setChecked(true);
    }
    widget->setLayout(layout);
    return widget;
}
bool WindPreferences::accept_config() {
    QSettings settings;
    settings.setValue(SETTINGS_WIND,group->checkedId());
    return true;
}
bool WindPreferences::is_configurable() {return true;}


WindModel::WindModel(){
}
WindModel::~WindModel(){
}
QWidget* WindModel::view_get_widget(){
    QWidget *widget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(new QLabel(QObject::tr("Channel")));
    layout->addWidget(new QLabel(QString::number(channel)));
    widget->setLayout(layout);
    return widget;
}
QWidget* WindModel::measurement_get_widget(){
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

    button = group->button(settings.value(SETTINGS_WIND,SETTINGS_WIND_DEFAULT).toInt());
    if(button != 0){
        button->setChecked(true);
    } else {
        group->buttons().first()->setChecked(true);
    }
    widget->setLayout(layout);
    return widget;
}
bool WindModel::measurement_accept_config(VariableModel *m){
        Q_UNUSED(m);
    channel = group->checkedId();
    // it really does not make sense to check unless it is included in the reference
/*
    if (m != NULL){
        if (channel != dynamic_cast<WindModel*>(m)->channel){
            QMessageBox message;
            message.setText("The channels are different.");
            message.exec();
        }
    }
*/
    return true;
}
bool WindModel::measurement_is_configurable(){
    return true;
}
void WindModel::save_xml(QDomElement root){
    QDomElement channel_element = root.ownerDocument().createElement(TAG_WIND_CHANNEL);
    channel_element.appendChild(root.ownerDocument().createTextNode(QString::number(this->channel)));
    root.appendChild(channel_element);
}
void WindModel::load_xml(QDomElement root){
    QDomNodeList nodelist = root.childNodes();
    QDomNode node;
    QDomElement element;
    for (int k=0; k< nodelist.count();k++){
        node = nodelist.at(k);
        element = node.toElement();
        if (element.tagName() == TAG_WIND_CHANNEL){
            this->channel = element.text().toInt();
            continue;
        }
    }
}


WindHardware::WindHardware(VariableModel* v) {
    channel = dynamic_cast<WindModel*>(v)->channel;
}
WindHardware::~WindHardware(){
}
void WindHardware::read() { raw = (1.0 * qrand() / RAND_MAX); value = 10 *raw;}
double WindHardware::get_value(int n) {Q_UNUSED(n); return value;}
double WindHardware::get_raw_value(int n) {Q_UNUSED(n); return raw;}
void WindHardware::set_value(int n ,double value) {Q_UNUSED(n); Q_UNUSED(value);  }
bool WindHardware::isReady(void) {return true;}
bool WindHardware::has_set_final() {return meta->is_controlable() && false;}
void WindHardware::set_final() {}
void WindHardware::set_zero(QVector<double> zero) {Q_UNUSED(zero);}



VariableMeta* WindFactory::CreateVariableMeta() { return new WindMeta;}
VariablePreferences* WindFactory::CreateVariableGUI() { return new WindPreferences;}
VariableModel* WindFactory::CreateVariableModel() { return new WindModel;}
VariableHardware* WindFactory::CreateVariableHardware(VariableModel *v) { return new WindHardware(v);}


Q_EXPORT_PLUGIN2(40_wind, WindFactory);
