#include "measurementspreferences.h"
#include "ui_measurementspreferences.h"

#include "QDebug"
#include "QMessageBox"

MeasurementsPreferences::MeasurementsPreferences(MeasurementsModel *measurement, MeasureList *list, QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementsPreferences),
    measurement(measurement),
    list(list)
{
    ui->setupUi(this);

    ui->combo_matrix->clear();
    ui->combo_matrix->addItem("middle",MIDDLE);
    ui->combo_matrix->addItem("floor",FLOOR);
    int index = ui->combo_matrix->findData(settings->value("default_matrix").toInt());
    ui->combo_matrix->setCurrentIndex(index);

    ui->combo_dvm_time->addItem("50 ms",1);
    ui->combo_dvm_time->addItem("100 ms",2);
    ui->combo_dvm_time->addItem("500 ms",3);
    ui->combo_dvm_time->addItem("1 s",4);
    ui->combo_dvm_time->addItem("5 s",5);
    ui->combo_dvm_time->addItem("10 s",6);
    index = ui->combo_dvm_time->findData(settings->value("default_dvm_time").toInt());
    ui->combo_dvm_time->setCurrentIndex(index);

    ui->spinBox->setValue(settings->value("default_average_number").toInt());

    if (measurement->isZero){
        ui->doubleSpinBoxMax->setHidden(true);
        ui->doubleSpinBoxMin->setHidden(true);
        ui->doubleSpinBoxSettling->setHidden(true);
        ui->doubleSpinBoxStep->setHidden(true);
        ui->groupBox->setHidden(true);
        ui->labelMax->setHidden(true);
        ui->labelMin->setHidden(true);
        ui->labelStep->setHidden(true);
        ui->labelSettlingTime->setHidden(true);
        ui->comboBox->hide();
        ui->labelZero->hide();
    }else{
        ui->comboBox->setModel(list);
        ui->doubleSpinBoxSettling->setValue(settings->value("default_settling_time").toDouble());
        ui->controlGroup->setId(ui->radioButtonNone,NONE);
        ui->controlGroup->setId(ui->radioButtonAlpha,ALPHA);
        ui->controlGroup->setId(ui->radioButtonBeta,BETA);
        ui->controlGroup->setId(ui->radioButtonWind,WIND);

        connect(ui->controlGroup, SIGNAL(buttonClicked(int)),this,SLOT(maxminstep_enabled(int)));
    }
    adjustSize();
}


MeasurementsPreferences::~MeasurementsPreferences()
{
    delete ui;
}

void MeasurementsPreferences::on_buttonBox_accepted()
{
    measurement->name = ui->edit_name->text();
    measurement->matrix = (matrix_t) ui->combo_matrix->currentText().toInt();
    measurement->dvm_time = ui->combo_dvm_time->itemData(ui->combo_dvm_time->currentIndex()).toInt();
    measurement->average_number = ui->spinBox->value();
    measurement->settling_time = ui->doubleSpinBoxSettling->value();
    measurement->min = ui->doubleSpinBoxMin->value();
    measurement->max = ui->doubleSpinBoxMax->value();
    measurement->step = ui->doubleSpinBoxStep->value();
    if (measurement->isZero == true){
        measurement->control_type=NONE;
    }else{
        measurement->control_type = (control_types_t) ui->controlGroup->checkedId();
    }
    measurement->zero = list->at(ui->comboBox->currentIndex());

}

void MeasurementsPreferences::test_input(){
    if (ui->edit_name->text().size()==0){
        QMessageBox msgBox;
        msgBox.setText("Name is empty");
        msgBox.exec();
        return ;
    }

    if (ui->controlGroup->checkedId() !=NONE && measurement->isZero != true ){
        double min,max,step;

        min = ui->doubleSpinBoxMin->value();
        max = ui->doubleSpinBoxMax->value();
        step = ui->doubleSpinBoxStep->value();

        if( min >= max || max-min < step || step == 0  ){
            QMessageBox msgBox;
            msgBox.setText("Wrong Minimum, Maximum and Step");
            msgBox.exec();
            return ;
        }
    }

    emit MeasurementsPreferences::accept();
}

void MeasurementsPreferences::maxminstep_enabled(int id){
    double min,max,step;
    switch (id){
    case NONE:  ui->doubleSpinBoxMin->setEnabled(false);
                ui->doubleSpinBoxMax->setEnabled(false);
                ui->doubleSpinBoxStep->setEnabled(false);
                ui->doubleSpinBoxSettling->setEnabled(false);
                return;
    case ALPHA:
                min = DEFAULT_ALPHA_MIN;
                max = DEFAULT_ALPHA_MAX;
                step = DEFAULT_ALPHA_STEP;
                break;
    case BETA:
                min = DEFAULT_BETA_MIN;
                max = DEFAULT_BETA_MAX;
                step = DEFAULT_BETA_STEP;
                break;
    case WIND:
                min = DEFAULT_WIND_MIN;
                max = DEFAULT_WIND_MAX;
                step = DEFAULT_WIND_STEP;
                break;
    }

    ui->doubleSpinBoxMin->setEnabled(true);
    ui->doubleSpinBoxMax->setEnabled(true);
    ui->doubleSpinBoxStep->setEnabled(true);
    ui->doubleSpinBoxSettling->setEnabled(true);
    ui->doubleSpinBoxMin->setMinimum(min);
    ui->doubleSpinBoxMin->setMaximum(max);
    ui->doubleSpinBoxMin->setSingleStep(step);
    ui->doubleSpinBoxMax->setMinimum(min);
    ui->doubleSpinBoxMax->setMaximum(max);
    ui->doubleSpinBoxMax->setSingleStep(step);
    ui->doubleSpinBoxStep->setMinimum(min);
    ui->doubleSpinBoxStep->setMaximum(max);
    ui->doubleSpinBoxStep->setSingleStep(step);
    ui->doubleSpinBoxMin->setValue(min);
    ui->doubleSpinBoxMax->setValue(max);
    ui->doubleSpinBoxStep->setValue(step);

}
