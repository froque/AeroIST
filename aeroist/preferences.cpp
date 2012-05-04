#include "preferences.h"
#include "ui_preferences.h"

#include "common.h"
#include <QDir>
#include <QFileDialog>

#include <QPluginLoader>
#include "variable.h"

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

    Factory *factory;
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        factory = qobject_cast<Factory*>( loader.instance());
        if(factory){
            VariablePreferences * pref = factory->CreateVariableGUI();
            if (pref != NULL){
                variables.append( pref);
            }
        }
    }
    foreach (VariablePreferences *var, variables) {
        if(var->is_configurable()){
            ui->tabWidget->addTab(var->get_widget(),var->meta->get_general_name_tr());
        }
    }
    adjustSize();
}

Preferences::~Preferences(){
    delete ui;
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
