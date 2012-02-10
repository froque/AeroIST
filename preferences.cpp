#include "preferences.h"
#include "ui_preferences.h"

#include "common.h"
#include <QDir>
#include <QFileDialog>
#ifdef DEBUG
#include <QDebug>
#endif //DEBUG


#include "virtualvariables.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    QSettings settings;

    ui->edit_arduino->setText(      settings.value(SETTINGS_ARDUINO_PATH ).toString() );// to be deleted
    ui->edit_project->setText(settings.value(SETTINGS_PROJECT_FOLDER).toString());
    ui->spinBox->setValue(settings.value(SETTINGS_DEFAULT_AVERAGE_NUMBER).toInt());
    ui->doubleSpinBox->setValue(settings.value(SETTINGS_DEFAULT_SETTLING_TIME).toDouble());

    variables.append(new Virtual_TimeGUI);
    variables.append(new Virtual_ForceGUI);
    variables.append(new Virtual_AlphaGUI);
    variables.append(new Virtual_BetaGUI);
    variables.append(new Virtual_WindGUI);
    variables.append(new Virtual_MotorGUI);
    variables.append(new Virtual_TemperatureGUI);
    foreach (VariableGUI *var, variables) {
        if(var->is_configurable()){
            ui->tabWidget->addTab(var->get_config_widget(),var->meta->get_general_name());
        }
    }
}

Preferences::~Preferences(){
    delete ui;
}

void Preferences::on_buttonBox_accepted(){
    QSettings settings;
    settings.setValue(SETTINGS_ARDUINO_PATH,      ui->edit_arduino->text());  // to be deleted

    foreach (VariableGUI *var, variables) {
        if(var->is_configurable()){
            var->accept_config();
        }
    }

    settings.setValue(SETTINGS_DEFAULT_AVERAGE_NUMBER, ui->spinBox->value());
    settings.setValue(SETTINGS_DEFAULT_SETTLING_TIME,ui->doubleSpinBox->value());
    settings.setValue(SETTINGS_PROJECT_FOLDER,ui->edit_project->text());
    adjustSize();
}

void Preferences::on_toolButton_clicked(){
    QString directory;
    directory = QFileDialog::getExistingDirectory(this,tr("Choose Directory"),QDir::homePath(), QFileDialog::ShowDirsOnly);
    ui->edit_project->setText(directory);
}

// to be deleted - arduino
void Preferences::on_toolButton_5_clicked(){
    QString file;
    file = QFileDialog::getOpenFileName(this, tr("Choose device"),"/dev", "");
    ui->edit_arduino->setText(file);
}
