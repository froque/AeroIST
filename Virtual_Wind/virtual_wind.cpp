#include "virtual_wind.h"

#define NUM_CHANNELS 8
#define SETTINGS_WIND "wind_channel"
#define SETTINGS_WIND_DEFAULT 1

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
double WindMeta::get_lower_bound(int n) {Q_UNUSED(n); return 0;}
double WindMeta::get_upper_bound(int n) {Q_UNUSED(n); return 0;}
double WindMeta::get_smaller_step(int n) {Q_UNUSED(n); return 0;}
double WindMeta::get_default_step(int n) {Q_UNUSED(n); return 0;}
double WindMeta::get_default_start(int n) {Q_UNUSED(n); return 0;}


WindPreferences::WindPreferences() {meta = new WindMeta();}
QWidget* WindPreferences::get_widget() {
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    QAbstractButton *button;
    group = new QButtonGroup;
    for (int k=1; k<= NUM_CHANNELS; k++){
        button = new QRadioButton(QString(QObject::tr("channel %1")).arg(k));
        group->addButton(button);
        group->setId(button,k);
        layout->addWidget(button,k,0);
    }
    button = group->button(settings.value(SETTINGS_WIND,SETTINGS_WIND_DEFAULT).toInt());
    if(button != 0){
        button->setChecked(true);
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


WindModel::WindModel(){meta = new WindMeta;}
int WindModel::get_size() {return contents.size();}
double WindModel::get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
QVector<double> WindModel::get_vector(int n) {Q_UNUSED(n); return contents;}
void WindModel::set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
void WindModel::insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
void WindModel::append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
void WindModel::set_zero(QVector<double> zero) {Q_UNUSED(zero);}
QVector<double> WindModel::get_zero() {return QVector<double>();}
QWidget* WindModel::view_get_widget(){ return NULL;}
QWidget* WindModel::measurement_get_widget(){return NULL;}
bool WindModel::measurement_accept_config(VariableModel *m){Q_UNUSED(m); return true;}
bool WindModel::measurement_is_configurable(){return false;}
void WindModel::save_xml(QDomElement root){Q_UNUSED(root);}
void WindModel::load_xml(QDomElement root){Q_UNUSED(root);}
void WindModel::set_raw_value(int n, int row, double value){
    Q_UNUSED(n);
    raw.replace(row,value);
}
double WindModel::get_raw_value(int n, int row){
    Q_UNUSED(n);
    return raw.value(row);
}
void WindModel::insert_raw_value(int n, int row, int count, double value) {Q_UNUSED(n); raw.insert(row,count,value);}
void WindModel::append_raw_value(int n, double value) {Q_UNUSED(n);  raw.append(value);}

WindHardware::WindHardware() {meta = new WindMeta;}
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
VariableHardware* WindFactory::CreateVariableHardware(VariableModel *v) { Q_UNUSED(v); return new WindHardware;}


Q_EXPORT_PLUGIN2(40_wind, WindFactory);
