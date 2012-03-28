#include "referencedetails.h"
#include "ui_referencedetails.h"

ReferenceDetails::ReferenceDetails(ReferenceModel *measurement, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReferenceDetails)
{
    ui->setupUi(this);

    ui->labelName->setText( measurement->name);
    ui->labelDescription->setText(measurement->description);
    ui->labelMeasuresIteration->setText(QString::number(measurement->measures_per_iteration));

    ui->gridLayout->removeWidget(ui->buttonBox);
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
                label = new QLabel(var->meta->get_name(k).append(" (").append(var->meta->get_units(k)).append(")"));
                ui->verticalLayout->insertWidget(row,label);
                label = new QLabel(QString::number(var->start.at(k)));
                ui->verticalLayout_2->insertWidget(row,label);

                row++;
            }
        }
    }
    ui->gridLayout->addWidget(ui->buttonBox,row,1);
    this->adjustSize();
}

ReferenceDetails::~ReferenceDetails(){
    delete ui;
}
