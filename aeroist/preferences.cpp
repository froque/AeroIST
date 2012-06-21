#include "preferences.h"
#include "ui_preferences.h"

#include "common.h"
#include <QFileDialog>
#include "variable.h"
#include "pluginmanager.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    QSettings settings;

    ui->edit_project->setText(settings.value(SETTINGS_PROJECT_FOLDER).toString());
    ui->spinBox->setValue(settings.value(SETTINGS_DEFAULT_MEASURES_ITERATION).toInt());
    ui->doubleSpinBox->setValue(settings.value(SETTINGS_DEFAULT_SETTLING_TIME).toDouble());
    ui->checkBoxSchema->setChecked(settings.value(SETTINGS_SCHEMA_CONFIRM).toBool());

    PluginManager manager;
    variables = manager.getListVariablePreferences();

    foreach (VariablePreferences *var, variables) {
        if(var->is_configurable()){
            ui->tabWidget->addTab(var->get_widget(),var->meta->get_general_name_tr());
        }
    }
    if(settings.contains(SETTINGS_GUI_PREF_GEOMETRY)){
        restoreGeometry(settings.value(SETTINGS_GUI_PREF_GEOMETRY).toByteArray());
    } else {
        adjustSize();
    }
}

Preferences::~Preferences(){
    QSettings settings;
    settings.setValue(SETTINGS_GUI_PREF_GEOMETRY, saveGeometry());
    delete ui;
    PluginManager manager;
    manager.destroyListVariablePreferences(variables);
}

void Preferences::accept(){
    QSettings settings;

    foreach (VariablePreferences *var, variables) {
        if(var->is_configurable()){
            var->accept_config();
        }
    }

    settings.setValue(SETTINGS_DEFAULT_MEASURES_ITERATION, ui->spinBox->value());
    settings.setValue(SETTINGS_DEFAULT_SETTLING_TIME,ui->doubleSpinBox->value());
    settings.setValue(SETTINGS_PROJECT_FOLDER,ui->edit_project->text());
    settings.setValue(SETTINGS_SCHEMA_CONFIRM,ui->checkBoxSchema->isChecked());

    QDialog::accept();
}

void Preferences::on_toolButton_clicked(){
    QString directory;
    directory = QFileDialog::getExistingDirectory(this,tr("Choose Directory"),QDir::homePath(), QFileDialog::ShowDirsOnly);
    ui->edit_project->setText(directory);
}
