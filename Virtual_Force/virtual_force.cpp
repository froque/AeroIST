#include "virtual_force.h"



bool Virtual_ForceMeta::is_controlable() {return false;}
bool Virtual_ForceMeta::has_zero() {return true;}
int Virtual_ForceMeta::get_num() {return 6;}
QString Virtual_ForceMeta::get_general_name(){ return "Forces";}
QString Virtual_ForceMeta::get_name(int n){
    switch (n){
    case 0: return "Fx"; break;
    case 1: return "Fy"; break;
    case 2: return "Fz"; break;
    case 3: return "Mx"; break;
    case 4: return "My"; break;
    case 5: return "Mz"; break;
    }
    return "";
}
QString Virtual_ForceMeta::get_units(int n) {
    switch (n){
    case 0:
    case 1:
    case 2:
        return "N"; break;
    case 3:
    case 4:
    case 5:
        return "N.m"; break;
    }
    return "";
}
double Virtual_ForceMeta::get_lower_bound(int n) {Q_UNUSED(n); return 0;}
double Virtual_ForceMeta::get_upper_bound(int n) {Q_UNUSED(n); return 0;}
double Virtual_ForceMeta::get_smaller_step(int n) {Q_UNUSED(n); return 0;}
double Virtual_ForceMeta::get_default_step(int n) {Q_UNUSED(n); return 0;}
double Virtual_ForceMeta::get_default_start(int n) {Q_UNUSED(n); return 0;}


Virtual_ForceGUI::Virtual_ForceGUI(){meta = new Virtual_ForceMeta;}
QWidget* Virtual_ForceGUI::get_config_widget() {
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    layout->addWidget(new QLabel(QObject::tr("Multimeter")),0,0);
    edit = new QLineEdit(settings.value(SETTINGS_MULTIMETER_PATH, SETTINGS_MULTIMETER_PATH_DEFAULT).toString());
    layout->addWidget(edit,0,1);
    layout->addWidget(new QLabel(QObject::tr("Default Multimeter time")),1,0);
    combo_time = new QComboBox;
    combo_time->addItem(QObject::tr("50 ms"),    1);
    combo_time->addItem(QObject::tr("100 ms"),   2);
    combo_time->addItem(QObject::tr("500 ms"),   3);
    combo_time->addItem(QObject::tr("1 s"),      4);
    combo_time->addItem(QObject::tr("5 s"),      5);
    combo_time->addItem(QObject::tr("10 s"),     6);
    int index = combo_time->findData(settings.value(SETTINGS_DEFAULT_DVM_TIME).toInt());
    combo_time->setCurrentIndex(index);
    layout->addWidget(combo_time,1,1);
    layout->addWidget(new QLabel(QObject::tr("Default Matrix")),2,0);
    combo_matrix =  new QComboBox;
    combo_matrix->addItem(QObject::tr("middle"), MIDDLE);
    combo_matrix->addItem(QObject::tr("floor"),  FLOOR);
    index = combo_matrix->findData(settings.value(SETTINGS_DEFAULT_MATRIX).toInt());
    combo_matrix->setCurrentIndex(index);
    layout->addWidget(combo_matrix,2,1);
    widget->setLayout(layout);
    return widget;
}
bool Virtual_ForceGUI::accept_config() {
    QSettings settings;
    int index = combo_time->currentIndex();
    settings.setValue(SETTINGS_DEFAULT_DVM_TIME, combo_time->itemData(index).toInt());
    settings.setValue(SETTINGS_MULTIMETER_PATH, edit->text());
    index = combo_matrix->currentIndex();
    settings.setValue(SETTINGS_DEFAULT_MATRIX, combo_matrix->itemData(index).toInt());
    return true;
}
bool Virtual_ForceGUI::is_configurable() {return true;}



Virtual_ForceModel::Virtual_ForceModel(){meta = new Virtual_ForceMeta;}
int Virtual_ForceModel::get_size() {return force[0].size();}
double Virtual_ForceModel::get_value(int n,int row) {return force[n].value(row);}
QVector<double> Virtual_ForceModel::get_vector(int n) {return force[n];}
void Virtual_ForceModel::set_value(int n ,int row, double value) { force[n].replace(row,value);}
void Virtual_ForceModel::insert_value(int n, int row, int count, double value) {force[n].insert(row,count,value);}
void Virtual_ForceModel::append_value(int n, double value) { force[n].append(value);}
void Virtual_ForceModel::set_zero(QVector<double> zero) {this->zero = zero;}
QVector<double> Virtual_ForceModel::get_zero() {return zero;}


Virtual_ForceHardware::Virtual_ForceHardware(){meta = new Virtual_ForceMeta;}
void Virtual_ForceHardware::read() { for (int k=0; k<6; k++) {value[k] = ( 1.1 * (k+1) * qrand() / RAND_MAX )- zero.value(k);}}
double Virtual_ForceHardware::get_value(int n) { return value[n];}
void Virtual_ForceHardware::set_value(int n ,double value) {Q_UNUSED(n); Q_UNUSED(value);  }
bool Virtual_ForceHardware::isReady(void) {return true;}
bool Virtual_ForceHardware::has_set_final() {return meta->is_controlable() && false;}
void Virtual_ForceHardware::set_final() {}
void Virtual_ForceHardware::set_zero(QVector<double> zero) {this->zero = zero; qDebug() << this->zero;}


VariableMeta* ForceFactory::CreateVariableMeta() { return new Virtual_ForceMeta;}
VariableGUI* ForceFactory::CreateVariableGUI() { return new Virtual_ForceGUI;}
VariableModel* ForceFactory::CreateVariableModel() { return new Virtual_ForceModel;}
VariableHardware* ForceFactory::CreateVariableHardware() { return new Virtual_ForceHardware;}

Q_EXPORT_PLUGIN2(virtual_force, ForceFactory);
