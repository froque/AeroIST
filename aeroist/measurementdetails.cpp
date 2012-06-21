#include "measurementdetails.h"
#include "ui_measurementdetails.h"
#include <QSettings>

MeasurementDetails::MeasurementDetails(MeasurementsModel *measurement, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementDetails)
{
    ui->setupUi(this);

    // general tab
    ui->labelName->setText( measurement->name);
    ui->labelDescription->setText(measurement->description);
    ui->labelSettling->setText(QString::number(measurement->settling_time));
    ui->labelMeasuresIteration->setText(QString::number(measurement->measures_per_iteration));
    ui->labelReference->setText(measurement->ref_name);


    QLabel *label;
    QGridLayout *layout = new QGridLayout;
    int row = 0;

    label = new QLabel(tr("Control type"));
    layout->addWidget(label,row,0);

    if (measurement->control == ""){
        label = new QLabel(tr("None"));
        layout->addWidget(label,row,1);
        row++;
        label = new QLabel(tr("Iterations"));
        layout->addWidget(label,row,0);
        label = new QLabel(QString::number( measurement->iterations));
        layout->addWidget(label,row,1);
        row++;
    } else{
        label = new QLabel(measurement->control);
        layout->addWidget(label,row,1);
        row++;
        label = new QLabel(tr("End"));
        layout->addWidget(label,row,0);
        label = new QLabel(QString::number(measurement->end));
        layout->addWidget(label,row,1);
        row++;
        label = new QLabel(tr("Step"));
        layout->addWidget(label,row,0);
        label = new QLabel(QString::number(measurement->step));
        layout->addWidget(label,row,1);
        row++;
    }

    foreach (VariableModel *var, measurement->variables) {
        if(var->meta->is_controlable()){
            for (int k=0; k<var->meta->get_num(); k++){
                label = new QLabel(var->meta->get_name_tr(k).append(" (").append(var->meta->get_units(k)).append(")"));
                layout->addWidget(label,row,0);
                label = new QLabel(QString::number(var->start.at(k)));
                layout->addWidget(label,row,1);
                row++;
            }
        }
    }

    ui->tab_control->setLayout(layout);

    foreach (VariableModel *var, measurement->variables) {
        if(var->measurement_is_configurable()){
            ui->tabWidget->addTab(var->view_get_widget(),var->meta->get_general_name_tr());
        }
    }
    QSettings settings;
    if(settings.contains(SETTINGS_GUI_MD_GEOMETRY)){
        restoreGeometry(settings.value(SETTINGS_GUI_MD_GEOMETRY).toByteArray());
    } else {
        adjustSize();
    }
}

MeasurementDetails::~MeasurementDetails(){
    QSettings settings;
    settings.setValue(SETTINGS_GUI_MD_GEOMETRY, saveGeometry());
    delete ui;
}
