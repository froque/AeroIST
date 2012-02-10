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

    QLabel *label;
    int row = ui->verticalLayout->indexOf(ui->widget);
    foreach (VariableModel *var, measurement->variables) {
        if(var->meta->is_controlable()){
            for (int k=0; k<var->meta->get_num(); k++){
                label = new QLabel(var->meta->get_name(k).append(" (").append(var->meta->get_units(k)).append(")"));
                ui->verticalLayout->insertWidget(row,label);
                label = new QLabel(QString::number( measurement->start_hash[var->meta->get_name(k)]));
                ui->verticalLayout_2->insertWidget(row,label);

                row++;
            }
        }
    }
    this->adjustSize();
}

ZeroDetails::~ZeroDetails(){
    delete ui;
}
