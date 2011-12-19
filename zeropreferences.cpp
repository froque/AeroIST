#include "zeropreferences.h"
#include "ui_zeropreferences.h"

#include "QDebug"
#include "QMessageBox"

ZeroPreferences::ZeroPreferences(ZeroModel *measurement, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZeroPreferences),
    measurement(measurement)
{
    ui->setupUi(this);
    QSettings settings;
    ui->combo_matrix->clear();
    ui->combo_matrix->addItem(tr("middle"),MIDDLE);
    ui->combo_matrix->addItem(tr("floor"),FLOOR);
    int index = ui->combo_matrix->findData(settings.value(SETTINGS_DEFAULT_MATRIX).toInt());
    ui->combo_matrix->setCurrentIndex(index);

    ui->combo_dvm_time->addItem(tr("50 ms"),1);
    ui->combo_dvm_time->addItem(tr("100 ms"),2);
    ui->combo_dvm_time->addItem(tr("500 ms"),3);
    ui->combo_dvm_time->addItem(tr("1 s"),4);
    ui->combo_dvm_time->addItem(tr("5 s"),5);
    ui->combo_dvm_time->addItem(tr("10 s"),6);
    index = ui->combo_dvm_time->findData(settings.value(SETTINGS_DEFAULT_DVM_TIME).toInt());
    ui->combo_dvm_time->setCurrentIndex(index);

    ui->spinBoxAverage->setValue(settings.value(SETTINGS_DEFAULT_AVERAGE_NUMBER).toInt());
    ui->doubleSpinBoxAlpha->setRange(-ANGLEMAX_ALPHA,ANGLEMAX_ALPHA);
    ui->doubleSpinBoxAlpha->setSingleStep(DEFAULT_ALPHA_STEP);
    ui->doubleSpinBoxBeta->setRange(-ANGLEMAX_BETA,ANGLEMAX_BETA);
    ui->doubleSpinBoxBeta->setSingleStep(DEFAULT_BETA_STEP);
    ui->doubleSpinBoxWind->setRange(DEFAULT_WIND_MIN,DEFAULT_WIND_MAX);
    ui->doubleSpinBoxWind->setSingleStep(DEFAULT_WIND_STEP);
    adjustSize();

}


ZeroPreferences::~ZeroPreferences()
{
    delete ui;
}

//void ZeroPreferences::on_buttonBox_accepted(){

//}

//void ZeroPreferences::test_input(){
void ZeroPreferences::accept(){
    if (ui->edit_name->text().size()==0){
        QMessageBox msgBox;
        msgBox.setText(tr("Name is empty"));
        msgBox.exec();
        return ;
    }

//    emit ZeroPreferences::accept();

    measurement->name = ui->edit_name->text();
    measurement->description = ui->plainTextEdit->toPlainText();
    measurement->matrix = (matrix_t) ui->combo_matrix->currentIndex();
    measurement->dvm_time = ui->combo_dvm_time->itemData(ui->combo_dvm_time->currentIndex()).toInt();
    measurement->average_number = ui->spinBoxAverage->value();
    measurement->set_alpha = ui->doubleSpinBoxAlpha->value();
    measurement->set_beta = ui->doubleSpinBoxBeta->value();
    measurement->set_wind = ui->doubleSpinBoxWind->value();

    QDialog::accept();
}
