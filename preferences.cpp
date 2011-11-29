#include "preferences.h"
#include "ui_preferences.h"

#include "common.h"

#ifdef DEBUG
#include <QDebug>
#endif //DEBUG

Preferences::Preferences(QSettings *settings,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences),
  settings(settings)
{
    ui->setupUi(this);

    settings->setValue("alpha_path", settings->value("alpha_path","/dev/alpha"));
    ui->edit_alpha->setText(        settings->value("alpha_path").toString() );
    settings->setValue("beta_path", settings->value("beta_path","/dev/beta"));
    ui->edit_beta->setText(         settings->value("beta_path").toString() );
    settings->setValue("motor_path", settings->value("motor_path","/dev/ttyS0"));
    ui->edit_motor->setText(        settings->value("motor_path").toString() );
    settings->setValue("arduino_path", settings->value("arduino_path","/dev/ttyUSB0"));
    ui->edit_arduino->setText(      settings->value("arduino_path" ).toString() );
    settings->setValue("multimeter_path", settings->value("multimeter_path","/dev/gpib0"));
    ui->edit_multimetro->setText(   settings->value("multimeter_path").toString() );

    ui->combo_matrix->clear();
    ui->combo_matrix->addItem(tr("middle"), MIDDLE);
    ui->combo_matrix->addItem(tr("floor"),  FLOOR);
    settings->setValue("default_matrix", settings->value("default_matrix",MIDDLE));
    int index = ui->combo_matrix->findData(settings->value("default_matrix").toInt());
    ui->combo_matrix->setCurrentIndex(index);

    ui->combo_dvm_time->addItem(tr("50 ms"),    1);
    ui->combo_dvm_time->addItem(tr("100 ms"),   2);
    ui->combo_dvm_time->addItem(tr("500 ms"),   3);
    ui->combo_dvm_time->addItem(tr("1 s"),      4);
    ui->combo_dvm_time->addItem(tr("5 s"),      5);
    ui->combo_dvm_time->addItem(tr("10 s"),     6);
    settings->setValue("default_dvm_time", settings->value("default_dvm_time",4));
    index = ui->combo_dvm_time->findData(settings->value("default_dvm_time").toInt());
    ui->combo_dvm_time->setCurrentIndex(index);

    settings->setValue("default_average_number",settings->value("default_average_number",1));
    ui->spinBox->setValue(settings->value("default_average_number").toInt());

    settings->setValue("default_settling_time",settings->value("default_settling_time",0));
    ui->doubleSpinBox->setValue(settings->value("default_settling_time").toDouble());
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::on_buttonBox_accepted()
{
    settings->setValue("alpha_path",        ui->edit_alpha->text());
    settings->setValue("beta_path",         ui->edit_beta->text());
    settings->setValue("arduino_path",      ui->edit_arduino->text());
    settings->setValue("motor_path",        ui->edit_motor->text());
    settings->setValue("multimeter_path",   ui->edit_multimetro->text());

    int index = ui->combo_matrix->currentIndex();
    settings->setValue("default_matrix", ui->combo_matrix->itemData(index).toInt());

    index = ui->combo_dvm_time->currentIndex();
    settings->setValue("default_dvm_time", ui->combo_dvm_time->itemData(index).toInt());

    settings->setValue("default_average_number", ui->spinBox->value());
    settings->setValue("default_settling_time",ui->doubleSpinBox->value());
}

void Preferences::on_buttonBox_rejected()
{
    ui->edit_alpha->setText(        settings->value("alpha_path").toString() );
    ui->edit_beta->setText(         settings->value("beta_path").toString() );
    ui->edit_motor->setText(        settings->value("motor_path").toString() );
    ui->edit_arduino->setText(      settings->value("arduino_path" ).toString() );
    ui->edit_multimetro->setText(   settings->value("multimeter_path").toString() );

    int index = ui->combo_matrix->findData(settings->value("default_matrix").toInt());
    ui->combo_matrix->setCurrentIndex(index);

    index = ui->combo_dvm_time->findData(settings->value("default_dvm_time").toInt());
    ui->combo_dvm_time->setCurrentIndex(index);

    ui->spinBox->setValue(settings->value("default_average_number").toInt());
    ui->doubleSpinBox->setValue(settings->value("default_settling_time").toDouble());
}
