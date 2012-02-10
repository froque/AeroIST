#include "measurementspreferences.h"
#include "ui_measurementspreferences.h"

#include "QDebug"
#include "QMessageBox"
#include <cmath>
#include <QSettings>

#define COL_RADIO 1
#define COL_START 2
#define COL_END   3
#define COL_STEP  4


MeasurementsPreferences::MeasurementsPreferences(MeasurementsModel *measurement, ZeroList *list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementsPreferences),
    measurement(measurement),
    list(list)
{
    ui->setupUi(this);

    QSettings settings;

    proxyfilter = new QSortFilterProxyModel(this);
    proxyfilter->setSourceModel(list);
    proxyfilter->setFilterRole(Qt::UserRole);
    proxyfilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->combo_zero->setModel(proxyfilter);

    ui->combo_matrix->clear();
    ui->combo_matrix->addItem(tr("middle"),MIDDLE);
    ui->combo_matrix->addItem(tr("floor"),FLOOR);
    int index = ui->combo_matrix->findData(settings.value(SETTINGS_DEFAULT_MATRIX).toInt());
    ui->combo_matrix->setCurrentIndex(index);

    ui->combo_dvm_time->addItem(tr("50 ms"),1);
    ui->combo_dvm_time->addItem(tr("100 ms"),2);
    ui->combo_dvm_time->addItem(tr("500 ms"),3);
    ui->combo_dvm_time->addItem(tr("1 s"),4);
    ui->combo_dvm_time->addItem(tr("5 s"),5);
    ui->combo_dvm_time->addItem(tr("10 s"),6);
    index = ui->combo_dvm_time->findData(settings.value(SETTINGS_DEFAULT_DVM_TIME).toInt());
    ui->combo_dvm_time->setCurrentIndex(index);

    ui->spinBoxAverage->setValue(settings.value(SETTINGS_DEFAULT_AVERAGE_NUMBER).toInt());
    ui->doubleSpinBoxSettling->setValue(settings.value(SETTINGS_DEFAULT_SETTLING_TIME).toDouble());

    QGridLayout *layout = new QGridLayout;
    QLabel *label;
    QRadioButton *radio;
    QDoubleSpinBox *spin;

    int row = 0;

    group = new QButtonGroup(ui->widget);
    radio_none = new QRadioButton(tr("None"),ui->widget);
    radio_none->setChecked(true);
    layout->addWidget(radio_none,row,COL_RADIO);
    group->addButton(radio_none);
    label = new QLabel(trUtf8("Nº of iterations"),ui->widget);
    layout->addWidget(label,row,COL_START);          // not really a start spinbox, but just to be pretty
    spin_iterations = new QSpinBox(ui->widget);
    spin_iterations->setRange(0,1000);
    layout->addWidget(spin_iterations,row,COL_END);
    row++;
    label = new QLabel (tr("Start"));
    layout->addWidget(label,row,COL_START);
    label = new QLabel (tr("End"));
    layout->addWidget(label,row,COL_END);
    label = new QLabel (tr("Step"));
    layout->addWidget(label,row,COL_STEP);
    row++;
    foreach (VariableModel *var, measurement->variables) {
        if (var->meta->is_controlable()){
            for (int k=0; k< var->meta->get_num(); k++){
                radio = new QRadioButton(var->meta->get_name(k).append(" (").append(var->meta->get_units(k)).append(")"),ui->widget);
                radio->setObjectName(var->meta->get_name(k));
                list_radio.append(radio);
                layout->addWidget(radio, row, COL_RADIO);
                group->addButton(radio,row);
                spin = new QDoubleSpinBox(ui->widget);
                spin->setRange(var->meta->get_lower_bound(k),var->meta->get_upper_bound(k));
                spin->setSingleStep(var->meta->get_default_step(k));
                spin->setValue(var->meta->get_default_start(k));
                list_start.append(spin);
                layout->addWidget(spin,row,COL_START);
                spin = new QDoubleSpinBox(ui->widget);
                spin->setRange(var->meta->get_lower_bound(k),var->meta->get_upper_bound(k));
                spin->setSingleStep(var->meta->get_default_step(k));
                spin->setValue(var->meta->get_upper_bound(k));
                list_end.append(spin);
                layout->addWidget(spin,row,COL_END);
                spin = new QDoubleSpinBox(ui->widget);
                spin->setRange(var->meta->get_smaller_step(k),var->meta->get_upper_bound(k) - var->meta->get_lower_bound(k));
                spin->setSingleStep(var->meta->get_default_step(k));
                spin->setValue(var->meta->get_default_step(k));
                list_step.append(spin);
                layout->addWidget(spin,row,COL_STEP);
                row++;
            }
        }
    }
    ui->widget->setLayout(layout);
    ui->widget->adjustSize();

    connect(group, SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(maxminstep_enabled(QAbstractButton*)));
    ui->edit_name->setFocus();
    adjustSize();
}


MeasurementsPreferences::~MeasurementsPreferences(){
    delete ui;
    delete proxyfilter;
}

void MeasurementsPreferences::accept(){
    if (ui->edit_name->text().size()==0){
        QMessageBox msgBox;
        msgBox.setText(tr("Name is empty"));
        msgBox.exec();
        return ;
    }
    if (  ui->combo_zero->model()->rowCount(QModelIndex()) == 0){
        QMessageBox msgBox;
        msgBox.setText(tr("No zeros. Create on first"));
        msgBox.exec();
        return ;
    }

    double start = 0, end = 0,step = 0;
    // what is the desired control?
    if (group->checkedButton() == radio_none){
        measurement->control = "";
    } else {
        measurement->control = group->checkedButton()->objectName();
        int k = list_radio.indexOf( qobject_cast<QRadioButton*>(group->checkedButton()));
        start = list_start.value(k)->value();
        end = list_end.value(k)->value();
        step = list_step.value(k)->value();
        if( start > end ){
            step = -step;
        }
    }

    measurement->name = ui->edit_name->text();
    measurement->description = ui->plainTextEdit->toPlainText();
    measurement->matrix = (matrix_t) ui->combo_matrix->currentText().toInt();
    measurement->dvm_time = ui->combo_dvm_time->itemData(ui->combo_dvm_time->currentIndex()).toInt();
    measurement->average_number = ui->spinBoxAverage->value();
    measurement->settling_time = ui->doubleSpinBoxSettling->value();

    measurement->end = end;
    measurement->step = step;

    measurement->n = spin_iterations->value();
    for (int k=0; k<list_radio.size(); k++){
        measurement->start_hash[list_radio.value(k)->objectName()] = list_start.value(k)->value();
    }

    ZeroModel *zero = list->at(ui->combo_zero->currentIndex());
    for (int k=0; k < NFORCES; k++){
        measurement->zero[k] = zero->force[k].at(0);
    }
    measurement->zero_name = zero->name;
    QDialog::accept();
}

void MeasurementsPreferences::maxminstep_enabled(QAbstractButton* button) {
    QDoubleSpinBox *spin;
    foreach (spin, list_end) {
        spin->setEnabled(false);
    }
    foreach (spin, list_step) {
        spin->setEnabled(false);
    }
    if (button == radio_none){
        spin_iterations->setEnabled(true);
    } else {
        spin_iterations->setEnabled(false);
        int row = list_radio.indexOf( qobject_cast<QRadioButton*>(button));
        list_end.value(row)->setEnabled(true);
        list_step.value(row)->setEnabled(true);
    }
}

void MeasurementsPreferences::on_combo_matrix_currentIndexChanged(int index){
    proxyfilter->setFilterFixedString(QString::number(index));
}
