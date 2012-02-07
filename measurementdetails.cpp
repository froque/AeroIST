#include "measurementdetails.h"
#include "ui_measurementdetails.h"

MeasurementDetails::MeasurementDetails(MeasurementsModel *measurement, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementDetails)
{
    ui->setupUi(this);

    ui->labelName->setText( measurement->name);
    ui->labelDescription->setText(measurement->description);
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
    ui->labelZero->setText(measurement->zero_name);

    ui->labelAlpha->setText(QString::number(measurement->start_hash["Alpha"]));
    ui->labelBeta->setText(QString::number(measurement->start_hash["Beta"]));
    ui->labelMotor->setText(QString::number(measurement->start_hash["Motor"]));
    if (measurement->control == ""){
        ui->labelControl->setText(tr("None"));
        ui->labelViewEnd->hide();
        ui->labelViewStep->hide();
        ui->labelViewSettling->hide();
        ui->labelEnd->hide();
        ui->labelStep->hide();
        ui->labelSettling->hide();
        ui->labelN->setText(QString::number( measurement->n));
    } else{
        ui->labelEnd->setText(QString::number(measurement->end));
        ui->labelStep->setText(QString::number(measurement->step));
        ui->labelSettling->setText(QString::number(measurement->settling_time));
        ui->labelN->hide();
        ui->labelViewN->hide();
        ui->labelControl->setText(measurement->control);
        if (measurement->control == "Alpha") {
            ui->labelAlpha->hide();
            ui->labelViewAlpha->hide();
        } else if (measurement->control == "Beta") {
            ui->labelBeta->hide();
            ui->labelViewBeta->hide();
        } else if (measurement->control == "Motor") {
            ui->labelMotor->hide();
            ui->labelViewMotor->hide();
        }
    }

    this->adjustSize();

}

MeasurementDetails::~MeasurementDetails()
{
    delete ui;
}
