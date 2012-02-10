#include "zeropreferences.h"
#include "ui_zeropreferences.h"

#include "QDebug"
#include "QMessageBox"

ZeroPreferences::ZeroPreferences(ZeroModel *measurement, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZeroPreferences),
    measurement(measurement)
{
    ui->setupUi(this);
    QSettings settings;
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
    int num_controls = 0;
    foreach (VariableModel *var, measurement->variables) {
        if (var->meta->is_controlable()){
            for (int k=0; k< var->meta->get_num(); k++){
                num_controls++;
            }
        }
    }

    if(num_controls > 0){
        QLabel *label;
        QDoubleSpinBox *spin;
        ui->gridLayout->removeWidget(ui->buttonBox);
        int row = 7;
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
        ui->gridLayout->addWidget(ui->buttonBox,row,1);
    }
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

    measurement->name = ui->edit_name->text();
    measurement->description = ui->plainTextEdit->toPlainText();
    measurement->matrix = (matrix_t) ui->combo_matrix->currentIndex();
    measurement->dvm_time = ui->combo_dvm_time->itemData(ui->combo_dvm_time->currentIndex()).toInt();
    measurement->average_number = ui->spinBoxAverage->value();

    for (int k=0; k<list_start.size(); k++){
        measurement->start_hash[list_start.value(k)->objectName()] = list_start.value(k)->value();
    }


    QDialog::accept();
}
