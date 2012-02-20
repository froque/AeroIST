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
QWidget* Virtual_ForceGUI::get_widget() {
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
QWidget* Virtual_ForceModel::view_get_widget(){
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(QObject::tr("Multimeter time")),0,0);
    layout->addWidget(new QLabel(),0,1);

    switch (dvm_time){
    case 1:
        layout->addWidget(new QLabel(QObject::tr("50 ms")),0,1); break;
    case 2:
        layout->addWidget(new QLabel(QObject::tr("100 ms")),0,1); break;
    case 3:
        layout->addWidget(new QLabel(QObject::tr("500 ms")),0,1); break;
    case 4:
        layout->addWidget(new QLabel(QObject::tr("1 s")),0,1); break;
    case 5:
        layout->addWidget(new QLabel(QObject::tr("5 s")),0,1); break;
    case 6:
        layout->addWidget(new QLabel(QObject::tr("10 s")),0,1); break;
    }
    layout->addWidget(new QLabel(QObject::tr("Test Type")),1,0);
    switch (matrix){
    case MIDDLE:
        layout->addWidget(new QLabel(QObject::tr("Middle")),1,1); break;
    case FLOOR:
        layout->addWidget(new QLabel(QObject::tr("Floor")),1,1); break;
    }
    layout->addWidget(new QLabel(),1,1);
    widget->setLayout(layout);
    return widget;
}
QWidget* Virtual_ForceModel::measurement_get_widget(){
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    layout->addWidget(new QLabel(QObject::tr("Multimeter time")),0,0);
    combo_time = new QComboBox;
    combo_time->addItem(QObject::tr("50 ms"),    1);
    combo_time->addItem(QObject::tr("100 ms"),   2);
    combo_time->addItem(QObject::tr("500 ms"),   3);
    combo_time->addItem(QObject::tr("1 s"),      4);
    combo_time->addItem(QObject::tr("5 s"),      5);
    combo_time->addItem(QObject::tr("10 s"),     6);
    int index = combo_time->findData(settings.value(SETTINGS_DEFAULT_DVM_TIME).toInt());
    combo_time->setCurrentIndex(index);
    layout->addWidget(combo_time,0,1);
    layout->addWidget(new QLabel(QObject::tr("Test Type")),1,0);
    combo_matrix =  new QComboBox;
    combo_matrix->addItem(QObject::tr("middle"), MIDDLE);
    combo_matrix->addItem(QObject::tr("floor"),  FLOOR);
    index = combo_matrix->findData(settings.value(SETTINGS_DEFAULT_MATRIX).toInt());
    combo_matrix->setCurrentIndex(index);
    layout->addWidget(combo_matrix,1,1);
    widget->setLayout(layout);
    return widget;
}
bool Virtual_ForceModel::measurement_accept_config(VariableModel *m){
    dvm_time = combo_time->itemData(combo_time->currentIndex()).toInt();
    matrix = (matrix_t) combo_matrix->currentIndex();
    if (m != NULL){
        if (matrix != dynamic_cast<Virtual_ForceModel*>(m)->matrix){
            return false;
        }
    }
    return true;
}
bool Virtual_ForceModel::measurement_is_configurable(){return true;}
void Virtual_ForceModel::save_xml(QDomElement root){
    QDomElement dvm_time = root.ownerDocument().createElement(TAG_DVM_TIME);
    dvm_time.appendChild(root.ownerDocument().createTextNode(QString::number(this->dvm_time)));
    root.appendChild(dvm_time);

    QDomElement matrix = root.ownerDocument().createElement(TAG_MATRIX);
    matrix.appendChild(root.ownerDocument().createTextNode(QString::number(this->matrix)));
    root.appendChild(matrix);
}
void Virtual_ForceModel::load_xml(QDomElement root){
    QDomNodeList nodelist = root.childNodes();
    QDomNode node;
    QDomElement element;
    for (int k=0; k< nodelist.count();k++){
        node = nodelist.at(k);
        element = node.toElement();


        if (element.tagName() == TAG_DVM_TIME){
            this->dvm_time = element.text().toInt();
            continue;
        }
        if (element.tagName() == TAG_MATRIX){
            int m = element.text().toInt();
            switch (m){
            case FLOOR: this->matrix = FLOOR; break;
            case MIDDLE: this->matrix = MIDDLE; break;
            }
            continue;
        }
    }
}


Virtual_ForceHardware::Virtual_ForceHardware(VariableModel* v){
    meta = new Virtual_ForceMeta;
    dvm_time = dynamic_cast<Virtual_ForceModel*>(v)->dvm_time;
    matrix = dynamic_cast<Virtual_ForceModel*>(v)->matrix;
}
void Virtual_ForceHardware::read() { for (int k=0; k<6; k++) {value[k] = ( 1.1 * (k+1) * qrand() / RAND_MAX )- zero.value(k);}}
double Virtual_ForceHardware::get_value(int n) { return value[n];}
void Virtual_ForceHardware::set_value(int n ,double value) {Q_UNUSED(n); Q_UNUSED(value);  }
bool Virtual_ForceHardware::isReady(void) {return true;}
bool Virtual_ForceHardware::has_set_final() {return meta->is_controlable() && false;}
void Virtual_ForceHardware::set_final() {}
void Virtual_ForceHardware::set_zero(QVector<double> zero) {this->zero = zero; qDebug() << this->zero;}

VariableMeta* ForceFactory::CreateVariableMeta() { return new Virtual_ForceMeta;}
VariablePreferences* ForceFactory::CreateVariableGUI() { return new Virtual_ForceGUI;}
VariableModel* ForceFactory::CreateVariableModel() { return new Virtual_ForceModel;}
VariableHardware* ForceFactory::CreateVariableHardware(VariableModel *v) { return new Virtual_ForceHardware(v);}

Q_EXPORT_PLUGIN2(virtual_force, ForceFactory);
