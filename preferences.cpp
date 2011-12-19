#include "preferences.h"
#include "ui_preferences.h"

#include "common.h"
#include <QDir>
#include <QFileDialog>
#ifdef DEBUG
#include <QDebug>
#endif //DEBUG

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    QSettings settings;
    settings.setValue(SETTINGS_ALPHA_PATH, settings.value(SETTINGS_ALPHA_PATH,SETTINGS_ALPHA_PATH_DEFAULT));
    ui->edit_alpha->setText(        settings.value(SETTINGS_ALPHA_PATH).toString() );
    settings.setValue(SETTINGS_BETA_PATH, settings.value(SETTINGS_BETA_PATH,SETTINGS_BETA_PATH_DEFAULT));
    ui->edit_beta->setText(         settings.value(SETTINGS_BETA_PATH).toString() );
    settings.setValue(SETTINGS_MOTOR_PATH, settings.value(SETTINGS_MOTOR_PATH,SETTINGS_MOTOR_PATH_DEFAULT));
    ui->edit_motor->setText(        settings.value(SETTINGS_MOTOR_PATH).toString() );
    settings.setValue(SETTINGS_ARDUINO_PATH, settings.value(SETTINGS_ARDUINO_PATH,SETTINGS_ARDUINO_PATH_DEFAULT));
    ui->edit_arduino->setText(      settings.value(SETTINGS_ARDUINO_PATH ).toString() );
    settings.setValue(SETTINGS_MULTIMETER_PATH, settings.value(SETTINGS_MULTIMETER_PATH,SETTINGS_MULTIMETER_PATH_DEFAULT));
    ui->edit_multimetro->setText(   settings.value(SETTINGS_MULTIMETER_PATH).toString() );

    settings.setValue(SETTINGS_PROJECT_FOLDER,settings.value(SETTINGS_PROJECT_FOLDER,QDir::homePath()).toString());
    ui->edit_project->setText(settings.value(SETTINGS_PROJECT_FOLDER).toString());

    ui->combo_matrix->clear();
    ui->combo_matrix->addItem(tr("middle"), MIDDLE);
    ui->combo_matrix->addItem(tr("floor"),  FLOOR);
    settings.setValue(SETTINGS_DEFAULT_MATRIX, settings.value(SETTINGS_DEFAULT_MATRIX,MIDDLE));
    int index = ui->combo_matrix->findData(settings.value(SETTINGS_DEFAULT_MATRIX).toInt());
    ui->combo_matrix->setCurrentIndex(index);

    ui->combo_dvm_time->addItem(tr("50 ms"),    1);
    ui->combo_dvm_time->addItem(tr("100 ms"),   2);
    ui->combo_dvm_time->addItem(tr("500 ms"),   3);
    ui->combo_dvm_time->addItem(tr("1 s"),      4);
    ui->combo_dvm_time->addItem(tr("5 s"),      5);
    ui->combo_dvm_time->addItem(tr("10 s"),     6);
    settings.setValue(SETTINGS_DEFAULT_DVM_TIME, settings.value(SETTINGS_DEFAULT_DVM_TIME,4));
    index = ui->combo_dvm_time->findData(settings.value(SETTINGS_DEFAULT_DVM_TIME).toInt());
    ui->combo_dvm_time->setCurrentIndex(index);

    settings.setValue(SETTINGS_DEFAULT_AVERAGE_NUMBER,settings.value(SETTINGS_DEFAULT_AVERAGE_NUMBER,1));
    ui->spinBox->setValue(settings.value(SETTINGS_DEFAULT_AVERAGE_NUMBER).toInt());

    settings.setValue(SETTINGS_DEFAULT_SETTLING_TIME,settings.value(SETTINGS_DEFAULT_SETTLING_TIME,0));
    ui->doubleSpinBox->setValue(settings.value(SETTINGS_DEFAULT_SETTLING_TIME).toDouble());
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::on_buttonBox_accepted(){
    QSettings settings;
    settings.setValue(SETTINGS_ALPHA_PATH,        ui->edit_alpha->text());
    settings.setValue(SETTINGS_BETA_PATH,         ui->edit_beta->text());
    settings.setValue(SETTINGS_ARDUINO_PATH,      ui->edit_arduino->text());
    settings.setValue(SETTINGS_MOTOR_PATH,        ui->edit_motor->text());
    settings.setValue(SETTINGS_MULTIMETER_PATH,   ui->edit_multimetro->text());

    int index = ui->combo_matrix->currentIndex();
    settings.setValue(SETTINGS_DEFAULT_MATRIX, ui->combo_matrix->itemData(index).toInt());

    index = ui->combo_dvm_time->currentIndex();
    settings.setValue(SETTINGS_DEFAULT_DVM_TIME, ui->combo_dvm_time->itemData(index).toInt());

    settings.setValue(SETTINGS_DEFAULT_AVERAGE_NUMBER, ui->spinBox->value());
    settings.setValue(SETTINGS_DEFAULT_SETTLING_TIME,ui->doubleSpinBox->value());
    settings.setValue(SETTINGS_PROJECT_FOLDER,ui->edit_project->text());

}

void Preferences::on_toolButton_clicked(){
    QString directory;
    directory = QFileDialog::getExistingDirectory(this,tr("Choose Directory"),QDir::homePath(), QFileDialog::ShowDirsOnly);
    ui->edit_project->setText(directory);
}

void Preferences::on_toolButton_2_clicked(){
    QString file;
    file = QFileDialog::getOpenFileName(this, tr("Choose device"),"/dev", "");
    ui->edit_alpha->setText(file);
}

void Preferences::on_toolButton_3_clicked(){
    QString file;
    file = QFileDialog::getOpenFileName(this, tr("Choose device"),"/dev", "");
    ui->edit_beta->setText(file);
}

void Preferences::on_toolButton_4_clicked(){
    QString file;
    file = QFileDialog::getOpenFileName(this, tr("Choose device"),"/dev", "");
    ui->edit_motor->setText(file);
}

void Preferences::on_toolButton_5_clicked(){
    QString file;
    file = QFileDialog::getOpenFileName(this, tr("Choose device"),"/dev", "");
    ui->edit_arduino->setText(file);
}
