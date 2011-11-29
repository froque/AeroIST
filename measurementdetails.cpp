#include "measurementdetails.h"
#include "ui_measurementdetails.h"

MeasurementDetails::MeasurementDetails(MeasurementsModel *measurement, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementDetails)
{
    ui->setupUi(this);

    ui->labelName->setText( measurement->name);

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
    ui->labelAverage->setText(QString::number(measurement->average_number));

    if (measurement->isZero == true){
        ui->labelViewControl->hide();
        ui->labelViewMax->hide();
        ui->labelViewStep->hide();
        ui->labelViewSettling->hide();
        ui->labelViewMin->hide();
        ui->labelViewZero->hide();
        ui->labelControl->hide();
        ui->labelMax->hide();
        ui->labelStep->hide();
        ui->labelSettling->hide();
        ui->labelMin->hide();
        ui->labelZero->hide();
        ui->labelN->hide();
        ui->labelViewN->hide();

    } else {
        if (measurement->zero == NULL){
            ui->labelZero->setText(tr("None"));
        }else{
            ui->labelZero->setText(measurement->zero->name);
        }
        switch (measurement->control_type){
        case NONE:
            ui->labelControl->setText(tr("None"));
            ui->labelViewMin->hide();
            ui->labelViewMax->hide();
            ui->labelViewStep->hide();
            ui->labelViewSettling->hide();
            ui->labelMin->hide();
            ui->labelMax->hide();
            ui->labelStep->hide();
            ui->labelSettling->hide();
            ui->labelN->setText(QString::number( measurement->n));
            break;
        case ALPHA:
            ui->labelControl->setText(tr("Alpha"));
            ui->labelMin->setText(QString::number(measurement->min));
            ui->labelMax->setText(QString::number(measurement->max));
            ui->labelStep->setText(QString::number(measurement->step));
            ui->labelSettling->setText(QString::number(measurement->settling_time));
            ui->labelN->hide();
            ui->labelViewN->hide();
            break;
        case BETA:
            ui->labelControl->setText(tr("Beta"));
            ui->labelMin->setText(QString::number(measurement->min));
            ui->labelMax->setText(QString::number(measurement->max));
            ui->labelStep->setText(QString::number(measurement->step));
            ui->labelSettling->setText(QString::number(measurement->settling_time));
            ui->labelN->hide();
            ui->labelViewN->hide();
            break;
        case WIND:
            ui->labelControl->setText(tr("Wind"));
            ui->labelMin->setText(QString::number(measurement->min));
            ui->labelMax->setText(QString::number(measurement->max));
            ui->labelStep->setText(QString::number(measurement->step));
            ui->labelSettling->setText(QString::number(measurement->settling_time));
            ui->labelN->hide();
            ui->labelViewN->hide();
            break;
        }
    }
    this->adjustSize();

}

MeasurementDetails::~MeasurementDetails()
{
    delete ui;
}
