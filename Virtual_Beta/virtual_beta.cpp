#include "virtual_beta.h"

#define DEFAULT_BETA_STEP   5
#define DEFAULT_BETA_START  0
#define ANGLEZERO_BETA 80751.0
#define ANGLESENSITIVITY_BETA 0.00211000
#define ANGLEMAX_BETA 180.0
#define ANGLEMINSTEP_BETA 0.2
#define PRECISION_BETA 0.05


#define SETTINGS_BETA_PATH "beta_path"
#define SETTINGS_BETA_PATH_DEFAULT "/dev/angle_beta"


bool Virtual_BetaMeta::is_controlable() {return true;}
bool Virtual_BetaMeta::has_zero() {return false;}
int Virtual_BetaMeta::get_num() {return 1;}
QString Virtual_BetaMeta::get_general_name(){ return "Beta";}
QString Virtual_BetaMeta::get_name(int n){Q_UNUSED(n);  return "Beta"; }
QString Virtual_BetaMeta::get_units(int n) {Q_UNUSED(n); return QString::fromUtf8("°"); }
double Virtual_BetaMeta::get_lower_bound(int n) {Q_UNUSED(n); return -ANGLEMAX_BETA;}
double Virtual_BetaMeta::get_upper_bound(int n) {Q_UNUSED(n); return ANGLEMAX_BETA;}
double Virtual_BetaMeta::get_smaller_step(int n) {Q_UNUSED(n); return ANGLEMINSTEP_BETA;}
double Virtual_BetaMeta::get_default_step(int n) {Q_UNUSED(n); return DEFAULT_BETA_STEP;}
double Virtual_BetaMeta::get_default_start(int n) {Q_UNUSED(n); return DEFAULT_BETA_START;}


Virtual_BetaGUI::Virtual_BetaGUI() {meta = new Virtual_BetaMeta();}
QWidget* Virtual_BetaGUI::get_widget() {
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    layout->addWidget(new QLabel(QObject::tr("Beta device path")),0,0);
    edit_beta = new QLineEdit;
    edit_beta->setText(settings.value(SETTINGS_BETA_PATH).toString());
    layout->addWidget(edit_beta,0,1);
    widget->setLayout(layout);
    return widget;
}

bool Virtual_BetaGUI::accept_config() {
    QSettings settings;
    settings.setValue(SETTINGS_BETA_PATH, edit_beta->text());
    return true;
}
bool Virtual_BetaGUI::is_configurable() {return true;}


Virtual_BetaModel::Virtual_BetaModel(){meta = new Virtual_BetaMeta;}
int Virtual_BetaModel::get_size() {return contents.size();}
double Virtual_BetaModel::get_value(int n,int row) {Q_UNUSED(n); return contents.value(row);}
QVector<double> Virtual_BetaModel::get_vector(int n) {Q_UNUSED(n); return contents;}
void Virtual_BetaModel::set_value(int n ,int row, double value) {Q_UNUSED(n);  contents.insert(row,value);}
void Virtual_BetaModel::insert_value(int n, int row, int count, double value) {Q_UNUSED(n); contents.insert(row,count,value);}
void Virtual_BetaModel::append_value(int n, double value) {Q_UNUSED(n);  contents.append(value);}
void Virtual_BetaModel::set_zero(QVector<double> zero) {Q_UNUSED(zero);}
QVector<double> Virtual_BetaModel::get_zero() {return QVector<double> ();}
QWidget* Virtual_BetaModel::view_get_widget(){ return NULL;}
QWidget* Virtual_BetaModel::measurement_get_widget(){return NULL;}
bool Virtual_BetaModel::measurement_accept_config(VariableModel *m){Q_UNUSED(m); return true;}
bool Virtual_BetaModel::measurement_is_configurable(){return false;}
void Virtual_BetaModel::save_xml(QDomElement root){Q_UNUSED(root);}
void Virtual_BetaModel::load_xml(QDomElement root){Q_UNUSED(root);}


Virtual_BetaHardware::Virtual_BetaHardware () {meta = new Virtual_BetaMeta; value=0; control_set=false;}
void Virtual_BetaHardware::read() {if(control_set==false){ value = -10.0 * qrand() / RAND_MAX;}}
double Virtual_BetaHardware::get_value(int n) {Q_UNUSED(n); return value;}
void Virtual_BetaHardware::set_value(int n ,double value) {Q_UNUSED(n);  control_set=true; this->value = value;}
bool Virtual_BetaHardware::isReady(void) {return true;}
bool Virtual_BetaHardware::has_set_final() {return meta->is_controlable() && false;}
void Virtual_BetaHardware::set_final() {}
void Virtual_BetaHardware::set_zero(QVector<double> zero) {Q_UNUSED(zero);}


VariableMeta* BetaFactory::CreateVariableMeta() { return new Virtual_BetaMeta;}
VariablePreferences* BetaFactory::CreateVariableGUI() { return new Virtual_BetaGUI;}
VariableModel* BetaFactory::CreateVariableModel() { return new Virtual_BetaModel;}
VariableHardware* BetaFactory::CreateVariableHardware(VariableModel *v) {Q_UNUSED(v); return new Virtual_BetaHardware;}

Q_EXPORT_PLUGIN2(virtual_beta, BetaFactory);
