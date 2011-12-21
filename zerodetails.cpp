#include "zerodetails.h"
#include "ui_zerodetails.h"

ZeroDetails::ZeroDetails(ZeroModel *measurement, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZeroDetails)
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
    ui->labelDescription->setText(measurement->description);
    ui->labelAlpha->setText(QString::number(measurement->set_alpha));
    ui->labelBeta->setText(QString::number(measurement->set_beta));
    ui->labelMotor->setText(QString::number(measurement->set_motor));
    this->adjustSize();

}

ZeroDetails::~ZeroDetails(){
    delete ui;
}
