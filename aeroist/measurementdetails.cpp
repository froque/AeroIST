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
    ui->labelMeasuresIteration->setText(QString::number(measurement->measures_per_iteration));
    ui->labelReference->setText(measurement->ref_name);

    int row = ui->verticalLayout->indexOf(ui->widget);

    foreach (VariableModel *var, measurement->variables) {
        if(var->measurement_is_configurable()){
            ui->gridLayout->addWidget(var->view_get_widget(),row,0,1,2);
            row++;
        }
    }

    QLabel *label;
    foreach (VariableModel *var, measurement->variables) {
        if(var->meta->is_controlable()){
            for (int k=0; k<var->meta->get_num(); k++){
                label = new QLabel(var->meta->get_name(k).append(" (").append(var->meta->get_units(k)).append(")"),ui->widget);
                ui->verticalLayout->insertWidget(row,label);
                label = new QLabel(QString::number(var->start.at(k)));
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
        ui->labelIterations->setText(QString::number( measurement->iterations));
    } else{
        ui->labelControl->setText(measurement->control);
        ui->labelEnd->setText(QString::number(measurement->end));
        ui->labelStep->setText(QString::number(measurement->step));
        ui->labelIterations->hide();
        ui->labelViewIterations->hide();
    }

    this->adjustSize();
}

MeasurementDetails::~MeasurementDetails(){
    delete ui;
}
