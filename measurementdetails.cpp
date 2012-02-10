#include "measurementdetails.h"
#include "ui_measurementdetails.h"

MeasurementDetails::MeasurementDetails(MeasurementsModel *measurement, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementDetails)
{
    ui->setupUi(this);

    ui->labelName->setText( measurement->name);
    ui->labelDescription->setText(measurement->description);
    ui->labelSettling->setText(QString::number(measurement->settling_time));
    ui->labelAverage->setText(QString::number(measurement->average_number));
    ui->labelZero->setText(measurement->zero_name);

    switch (measurement->matrix){
    case MIDDLE:
        ui->labelMatrix->setText(tr("Middle")); break;
    case FLOOR:
        ui->labelMatrix->setText(tr("Floor")); break;
    }

    switch (measurement->dvm_time){
    case 1:
        ui->labelMultimeter->setText(tr("50 ms")); break;
    case 2:
        ui->labelMultimeter->setText(tr("100 ms")); break;
    case 3:
        ui->labelMultimeter->setText(tr("500 ms")); break;
    case 4:
        ui->labelMultimeter->setText(tr("1 s")); break;
    case 5:
        ui->labelMultimeter->setText(tr("5 s")); break;
    case 6:
        ui->labelMultimeter->setText(tr("10 s")); break;
    }

    QLabel *label;

    int row;
    row = ui->verticalLayout->indexOf(ui->widget);
    foreach (VariableModel *var, measurement->variables) {
        if(var->meta->is_controlable()){
            for (int k=0; k<var->meta->get_num(); k++){
                label = new QLabel(var->meta->get_name(k).append(" (").append(var->meta->get_units(k)).append(")"),ui->widget);
                ui->verticalLayout->insertWidget(row,label);
                label = new QLabel(QString::number( measurement->start_hash[var->meta->get_name(k)]),ui->widget);
                ui->verticalLayout_2->insertWidget(row,label);

                row++;
            }
        }
    }

    if (measurement->control == ""){
        ui->labelControl->setText(tr("None"));
        ui->labelViewEnd->hide();
        ui->labelViewStep->hide();
        ui->labelEnd->hide();
        ui->labelStep->hide();
        ui->labelN->setText(QString::number( measurement->n));
    } else{
        ui->labelControl->setText(measurement->control);
        ui->labelEnd->setText(QString::number(measurement->end));
        ui->labelStep->setText(QString::number(measurement->step));
        ui->labelN->hide();
        ui->labelViewN->hide();
    }

    this->adjustSize();
}

MeasurementDetails::~MeasurementDetails(){
    delete ui;
}
