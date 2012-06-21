#include "referencepreferences.h"
#include "ui_referencepreferences.h"

#include <QMessageBox>
#include <QDir>
#include <QCoreApplication>
#include <QPluginLoader>

ReferencePreferences::ReferencePreferences(ReferenceModel *measurement, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReferencePreferences),
    measurement(measurement)
{
    ui->setupUi(this);
    QSettings settings;

    ui->spinBoxMeasuresIteration->setValue(settings.value(SETTINGS_DEFAULT_MEASURES_ITERATION).toInt());

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
        QLabel *label;
        QDoubleSpinBox *spin;
        QGridLayout *layout = new QGridLayout;
        int row = 0;
        foreach (VariableModel *var, measurement->variables) {
            if (var->meta->is_controlable()){
                for (int k=0; k< var->meta->get_num(); k++){
                    label = new QLabel (var->meta->get_name_tr(k).append(" (").append(var->meta->get_units(k)).append(")"));
                    layout->addWidget(label,row,0);
                    spin = new QDoubleSpinBox;
                    spin->setRange(var->meta->get_lower_bound(k),var->meta->get_upper_bound(k));
                    spin->setSingleStep(var->meta->get_default_step(k));
                    spin->setValue(var->meta->get_default_start(k));
                    spin->setObjectName(var->meta->get_name(k));
                    list_start.append(spin);
                    layout->addWidget(spin,row,1);
                    row++;
                }
            }
        }
        ui->tab_control->setLayout(layout);
    }

    foreach (VariableModel *var, measurement->variables) {
        if(var->measurement_is_configurable()){
            ui->tabWidget->addTab(var->measurement_get_widget(),var->meta->get_general_name_tr());
        }
    }
    ui->edit_name->setFocus();

    if(settings.contains(SETTINGS_GUI_RP_GEOMETRY)){
        restoreGeometry(settings.value(SETTINGS_GUI_RP_GEOMETRY).toByteArray());
    } else {
        adjustSize();
    }
}

ReferencePreferences::~ReferencePreferences(){
    QSettings settings;
    settings.setValue(SETTINGS_GUI_RP_GEOMETRY, saveGeometry());
    delete ui;
}

void ReferencePreferences::accept(){
    if (ui->edit_name->text().size()==0){
        QMessageBox msgBox;
        msgBox.setText(tr("Name is empty"));
        msgBox.exec();
        return ;
    }

    foreach (VariableModel *var, measurement->variables) {
        if (var->measurement_is_configurable()){
            if (var->measurement_accept_config(NULL) == false){
                return;
            }
        }
    }

    measurement->name = ui->edit_name->text();
    measurement->description = ui->plainTextEdit->toPlainText();
    measurement->measures_per_iteration = ui->spinBoxMeasuresIteration->value();

    foreach (VariableModel *var, measurement->variables) {
        if (var->meta->is_controlable()){
            var->start = QVector<double>(var->meta->get_num());
            for (int l=0; l<list_start.size(); l++){
                for (int k=0; k< var->meta->get_num(); k++){
                    if(var->meta->get_name(k) == list_start.value(l)->objectName()){
                        var->start[k] = list_start.value(l)->value();
                    }
                }
            }
        }
    }

    QDialog::accept();
}
