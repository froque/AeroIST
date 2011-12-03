#include "zeropreferences.h"
#include "ui_zeropreferences.h"

#include "QDebug"
#include "QMessageBox"

ZeroPreferences::ZeroPreferences(ZeroModel *measurement, QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZeroPreferences),
    measurement(measurement)
{
    ui->setupUi(this);

    ui->combo_matrix->clear();
    ui->combo_matrix->addItem(tr("middle"),MIDDLE);
    ui->combo_matrix->addItem(tr("floor"),FLOOR);
    int index = ui->combo_matrix->findData(settings->value("default_matrix").toInt());
    ui->combo_matrix->setCurrentIndex(index);

    ui->combo_dvm_time->addItem(tr("50 ms"),1);
    ui->combo_dvm_time->addItem(tr("100 ms"),2);
    ui->combo_dvm_time->addItem(tr("500 ms"),3);
    ui->combo_dvm_time->addItem(tr("1 s"),4);
    ui->combo_dvm_time->addItem(tr("5 s"),5);
    ui->combo_dvm_time->addItem(tr("10 s"),6);
    index = ui->combo_dvm_time->findData(settings->value("default_dvm_time").toInt());
    ui->combo_dvm_time->setCurrentIndex(index);

    ui->spinBoxAverage->setValue(settings->value("default_average_number").toInt());

    adjustSize();

}


ZeroPreferences::~ZeroPreferences()
{
    delete ui;
}

void ZeroPreferences::on_buttonBox_accepted(){
    measurement->name = ui->edit_name->text();
    measurement->matrix = (matrix_t) ui->combo_matrix->currentText().toInt();
    measurement->dvm_time = ui->combo_dvm_time->itemData(ui->combo_dvm_time->currentIndex()).toInt();
    measurement->average_number = ui->spinBoxAverage->value();
}

void ZeroPreferences::test_input(){
    if (ui->edit_name->text().size()==0){
        QMessageBox msgBox;
        msgBox.setText(tr("Name is empty"));
        msgBox.exec();
        return ;
    }

    emit ZeroPreferences::accept();
}
