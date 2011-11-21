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
        ui->labelMatrix->setText("Middle" ); break;
    case FLOOR:
        ui->labelMatrix->setText("Floor"); break;
    }

    switch (measurement->dvm_time){
    case 1:
        ui->labelMultimeter->setText("50 ms"); break;
    case 2:
        ui->labelMultimeter->setText("100 ms"); break;
    case 3:
        ui->labelMultimeter->setText("500 ms"); break;
    case 4:
        ui->labelMultimeter->setText("1 s"); break;
    case 5:
        ui->labelMultimeter->setText("5 s"); break;
    case 6:
        ui->labelMultimeter->setText("10 s"); break;
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

    } else {
        ui->labelZero->setText(measurement->zero->name);
        switch (measurement->control_type){
        case NONE:
            ui->labelControl->setText("None");
            ui->labelViewMin->hide();
            ui->labelViewMax->hide();
            ui->labelViewStep->hide();
            ui->labelViewSettling->hide();
            ui->labelMin->hide();
            ui->labelMax->hide();
            ui->labelStep->hide();
            ui->labelSettling->hide();
            break;
        case ALPHA:
            ui->labelControl->setText("Alpha");
            ui->labelMin->setText(QString::number(measurement->min));
            ui->labelMax->setText(QString::number(measurement->max));
            ui->labelStep->setText(QString::number(measurement->step));
            ui->labelSettling->setText(QString::number(measurement->settling_time));
            break;
        case BETA:
            ui->labelControl->setText("Beta");
            ui->labelMin->setText(QString::number(measurement->min));
            ui->labelMax->setText(QString::number(measurement->max));
            ui->labelStep->setText(QString::number(measurement->step));
            ui->labelSettling->setText(QString::number(measurement->settling_time));
            break;
        case WIND:
            ui->labelControl->setText("Wind");
            ui->labelMin->setText(QString::number(measurement->min));
            ui->labelMax->setText(QString::number(measurement->max));
            ui->labelStep->setText(QString::number(measurement->step));
            ui->labelSettling->setText(QString::number(measurement->settling_time));
            break;
        }
    }
    this->adjustSize();

}

MeasurementDetails::~MeasurementDetails()
{
    delete ui;
}
