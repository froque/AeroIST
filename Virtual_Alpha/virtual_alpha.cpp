#include "virtual_alpha.h"


bool Virtual_AlphaMeta::is_controlable() {return true;}
bool Virtual_AlphaMeta::has_zero() {return false;}
int Virtual_AlphaMeta::get_num() {return 1;}
QString Virtual_AlphaMeta::get_general_name(){ return "Alpha";}
QString Virtual_AlphaMeta::get_name(int n){Q_UNUSED(n);  return "Alpha"; }
QString Virtual_AlphaMeta::get_units(int n) { Q_UNUSED(n); return QString::fromUtf8("°"); }
double Virtual_AlphaMeta::get_lower_bound(int n) {Q_UNUSED(n); return -ANGLEMAX_ALPHA;}
double Virtual_AlphaMeta::get_upper_bound(int n) {Q_UNUSED(n); return ANGLEMAX_ALPHA;}
double Virtual_AlphaMeta::get_smaller_step(int n) {Q_UNUSED(n); return ANGLEMINSTEP_ALPHA;}
double Virtual_AlphaMeta::get_default_step(int n) {Q_UNUSED(n); return DEFAULT_ALPHA_STEP;}
double Virtual_AlphaMeta::get_default_start(int n) {Q_UNUSED(n); return DEFAULT_ALPHA_START;}


Virtual_AlphaGUI::Virtual_AlphaGUI() {meta = new Virtual_AlphaMeta();}
QWidget* Virtual_AlphaGUI::get_config_widget() {
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    layout->addWidget(new QLabel(QObject::tr("Alpha device path")),0,0);
    edit_alpha = new QLineEdit;
    edit_alpha->setText(settings.value(SETTINGS_ALPHA_PATH).toString());
    layout->addWidget(edit_alpha,0,1);
    widget->setLayout(layout);
    return widget;
}

bool Virtual_AlphaGUI::accept_config() {
    QSettings settings;
    settings.setValue(SETTINGS_ALPHA_PATH, edit_alpha->text());
    return true;
}
bool Virtual_AlphaGUI::is_configurable() {return true;}


Virtual_AlphaModel::Virtual_AlphaModel(){meta = new Virtual_AlphaMeta;}
int Virtual_AlphaModel::get_size() {return contents.size();}
double Virtual_AlphaModel::get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
QVector<double> Virtual_AlphaModel::get_vector(int n) {Q_UNUSED(n); return contents;}
void Virtual_AlphaModel::set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.replace(row,value);}
void Virtual_AlphaModel::insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
void Virtual_AlphaModel::append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
void Virtual_AlphaModel::set_zero(QVector<double> zero) {Q_UNUSED(zero);}
QVector<double> Virtual_AlphaModel::get_zero() {return QVector<double>();}


Virtual_AlphaHardware::Virtual_AlphaHardware(){meta = new Virtual_AlphaMeta; value=0; control_set=false;}
void Virtual_AlphaHardware::read() {if(control_set==false){ value = -10.0 * qrand() / RAND_MAX;}}
double Virtual_AlphaHardware::get_value(int n) {Q_UNUSED(n); return value;}
void Virtual_AlphaHardware::set_value(int n ,double value) {Q_UNUSED(n);  control_set=true; this->value = value;}
bool Virtual_AlphaHardware::isReady(void) {return true;}
bool Virtual_AlphaHardware::has_set_final() {return meta->is_controlable() && false;}
void Virtual_AlphaHardware::set_final() {}
void Virtual_AlphaHardware::set_zero(QVector<double> zero) {Q_UNUSED(zero);}


VariableMeta* AlphaFactory::CreateVariableMeta() { return new Virtual_AlphaMeta;}
VariableGUI* AlphaFactory::CreateVariableGUI() { return new Virtual_AlphaGUI;}
VariableModel* AlphaFactory::CreateVariableModel() { return new Virtual_AlphaModel;}
VariableHardware* AlphaFactory::CreateVariableHardware() { return new Virtual_AlphaHardware;}

Q_EXPORT_PLUGIN2(virtual_alpha, AlphaFactory);
