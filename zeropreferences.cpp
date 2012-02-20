#include "zeropreferences.h"
#include "ui_zeropreferences.h"


#include "QMessageBox"

#include <QDir>
#include <QCoreApplication>
#include <QPluginLoader>

ZeroPreferences::ZeroPreferences(ZeroModel *measurement, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZeroPreferences),
    measurement(measurement)
{
    ui->setupUi(this);
    QSettings settings;

    ui->spinBoxAverage->setValue(settings.value(SETTINGS_DEFAULT_AVERAGE_NUMBER).toInt());

    ui->gridLayout->removeWidget(ui->buttonBox);
    int row = 7;

    foreach (VariableModel *var, measurement->variables) {
        if(var->measurement_is_configurable()){
            ui->gridLayout->addWidget(var->measurement_get_widget(),row,0,1,2);
            row++;
        }
    }

    // just to count number of controllables
    int num_controls = 0;
    foreach (VariableModel *var, measurement->variables) {
        if (var->meta->is_controlable()){
            num_controls += var->meta->get_num();
        }
    }

    if(num_controls > 0){
        QLabel *label;
        QDoubleSpinBox *spin;
        foreach (VariableModel *var, measurement->variables) {
            if (var->meta->is_controlable()){
                for (int k=0; k< var->meta->get_num(); k++){
                    label = new QLabel (var->meta->get_name(k).append(" (").append(var->meta->get_units(k)).append(")"));
                    ui->gridLayout->addWidget(label,row,0);
                    spin = new QDoubleSpinBox(ui->widget);
                    spin->setRange(var->meta->get_lower_bound(k),var->meta->get_upper_bound(k));
                    spin->setSingleStep(var->meta->get_default_step(k));
                    spin->setValue(var->meta->get_default_start(k));
                    spin->setObjectName(var->meta->get_name(k));
                    list_start.append(spin);
                    ui->gridLayout->addWidget(spin,row,1);
                    row++;
                }
            }
        }
    }
    ui->gridLayout->addWidget(ui->buttonBox,row,1);
    ui->edit_name->setFocus();
    adjustSize();
}

ZeroPreferences::~ZeroPreferences(){
    delete ui;
}

void ZeroPreferences::accept(){
    if (ui->edit_name->text().size()==0){
        QMessageBox msgBox;
        msgBox.setText(tr("Name is empty"));
        msgBox.exec();
        return ;
    }

    foreach (VariableModel *var, measurement->variables) {
        if (var->measurement_is_configurable()){
            if (var->measurement_accept_config(NULL) == false){
                return;
            }
        }
    }

    measurement->name = ui->edit_name->text();
    measurement->description = ui->plainTextEdit->toPlainText();
    measurement->average_number = ui->spinBoxAverage->value();

    for (int k=0; k<list_start.size(); k++){
        measurement->start_hash[list_start.value(k)->objectName()] = list_start.value(k)->value();
    }
    QDialog::accept();
}
