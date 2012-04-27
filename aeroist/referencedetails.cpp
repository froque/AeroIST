#include "referencedetails.h"
#include "ui_referencedetails.h"

ReferenceDetails::ReferenceDetails(ReferenceModel *measurement, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReferenceDetails)
{
    ui->setupUi(this);

    // general tab fields
    ui->labelName->setText( measurement->name);
    ui->labelDescription->setText(measurement->description);
    ui->labelMeasuresIteration->setText(QString::number(measurement->measures_per_iteration));

    // just to count number of controllables
    int num_controls = 0;
    foreach (VariableModel *var, measurement->variables) {
        if (var->meta->is_controlable()){
            num_controls += var->meta->get_num();
        }
    }

    if(num_controls < 0 ){
        // remove empty tab
        ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tab_control));
    } else {
        QGridLayout *layout = new QGridLayout;
        int row = 0;
        QLabel *label;
        foreach (VariableModel *var, measurement->variables) {
            if(var->meta->is_controlable()){
                for (int k=0; k<var->meta->get_num(); k++){
                    label = new QLabel(var->meta->get_name_tr(k).append(" (").append(var->meta->get_units(k)).append(")"));
                    //                ui->verticalLayout->insertWidget(row,label);
                    layout->addWidget(label,row,0);
                    label = new QLabel(QString::number(var->start.at(k)));
                    //                ui->verticalLayout_2->insertWidget(row,label);
                    layout->addWidget(label,row,1);

                    row++;
                }
            }
        }
        ui->tab_control->setLayout(layout);
    }

    foreach (VariableModel *var, measurement->variables) {
        if(var->measurement_is_configurable()){
            ui->tabWidget->addTab(var->view_get_widget(),var->meta->get_general_name_tr());
        }
    }

    this->adjustSize();
}

ReferenceDetails::~ReferenceDetails(){
    delete ui;
}
