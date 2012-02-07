#include "measurementspreferences.h"
#include "ui_measurementspreferences.h"

#include "QDebug"
#include "QMessageBox"
#include <cmath>

MeasurementsPreferences::MeasurementsPreferences(MeasurementsModel *measurement, ZeroList *list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementsPreferences),
    measurement(measurement),
    list(list)
{
    ui->setupUi(this);

    QSettings settings;

    proxyfilter = new QSortFilterProxyModel(this);
    proxyfilter->setSourceModel(list);
    proxyfilter->setFilterRole(Qt::UserRole);
    proxyfilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->combo_zero->setModel(proxyfilter);

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

    ui->doubleSpinBoxSettling->setValue(settings.value(SETTINGS_DEFAULT_SETTLING_TIME).toDouble());
    //Alpha
    ui->doubleSpinBoxAlphaStart->setRange(-ANGLEMAX_ALPHA,ANGLEMAX_ALPHA);
    ui->doubleSpinBoxAlphaStart->setSingleStep(DEFAULT_ALPHA_STEP);
    ui->doubleSpinBoxAlphaEnd->setRange(-ANGLEMAX_ALPHA, ANGLEMAX_ALPHA);
    ui->doubleSpinBoxAlphaEnd->setSingleStep(DEFAULT_ALPHA_STEP);
    ui->doubleSpinBoxAlphaStep->setRange(-ANGLEMAX_ALPHA, ANGLEMAX_ALPHA);
    ui->doubleSpinBoxAlphaStep->setSingleStep(DEFAULT_ALPHA_STEP);
    //Beta
    ui->doubleSpinBoxBetaStart->setRange(-ANGLEMAX_BETA,ANGLEMAX_BETA);
    ui->doubleSpinBoxBetaStart->setSingleStep(DEFAULT_BETA_STEP);
    ui->doubleSpinBoxBetaEnd->setRange(-ANGLEMAX_BETA,ANGLEMAX_BETA);
    ui->doubleSpinBoxBetaEnd->setSingleStep(DEFAULT_BETA_STEP);
    ui->doubleSpinBoxBetaStep->setRange(-ANGLEMAX_BETA,ANGLEMAX_BETA);
    ui->doubleSpinBoxBetaStep->setSingleStep(DEFAULT_BETA_STEP);
    //Motor
    ui->doubleSpinBoxMotorStart->setRange(DEFAULT_MOTOR_MIN,DEFAULT_MOTOR_MAX);
    ui->doubleSpinBoxMotorStart->setSingleStep(DEFAULT_MOTOR_STEP);
    ui->doubleSpinBoxMotorEnd->setRange(DEFAULT_MOTOR_MIN,DEFAULT_MOTOR_MAX);
    ui->doubleSpinBoxMotorEnd->setSingleStep(DEFAULT_MOTOR_STEP);
    ui->doubleSpinBoxMotorStep->setRange(DEFAULT_MOTOR_MIN,DEFAULT_MOTOR_MAX);
    ui->doubleSpinBoxMotorStep->setSingleStep(DEFAULT_MOTOR_STEP);

    ui->controlGroup->setId(ui->radioButtonNone,NONE);
    ui->controlGroup->setId(ui->radioButtonAlpha,ALPHA);
    ui->controlGroup->setId(ui->radioButtonBeta,BETA);
    ui->controlGroup->setId(ui->radioButtonMotor,MOTOR);

    connect(ui->controlGroup, SIGNAL(buttonClicked(int)),this,SLOT(maxminstep_enabled(int)));
    ui->edit_name->setFocus();
    adjustSize();
}


MeasurementsPreferences::~MeasurementsPreferences(){
    delete ui;
    delete proxyfilter;
}

void MeasurementsPreferences::accept(){
    if (ui->edit_name->text().size()==0){
        QMessageBox msgBox;
        msgBox.setText(tr("Name is empty"));
        msgBox.exec();
        return ;
    }
    if (  ui->combo_zero->model()->rowCount(QModelIndex()) == 0){
        QMessageBox msgBox;
        msgBox.setText(tr("No zeros. Create on first"));
        msgBox.exec();
        return ;
    }
//    if (ui->controlGroup->checkedId() !=NONE ){
//        double start,end,step;


//        start = ui->doubleSpinBoxStart->value();
//        end = ui->doubleSpinBoxEnd->value();
//        step = ui->doubleSpinBoxStep->value();

//        if( (start > end && step > 0 ) || fabs(end-start) < fabs(step) || step == 0  ){
//            QMessageBox msgBox;
//            msgBox.setText(tr("Wrong Minimum, Maximum and Step"));
//            msgBox.exec();
//            return ;
//        }
//    }
    double start,end,step;
    switch (ui->controlGroup->checkedId()){
    case 0:
        measurement->control = ""; break;
    case 1:
        measurement->control = "Alpha";
        start = ui->doubleSpinBoxAlphaStart->value();
        end = ui->doubleSpinBoxAlphaEnd->value();
        step = ui->doubleSpinBoxAlphaStep->value();
        break;
    case 2:
        measurement->control = "Beta";
        start = ui->doubleSpinBoxBetaStart->value();
        end = ui->doubleSpinBoxBetaEnd->value();
        step = ui->doubleSpinBoxBetaStep->value();
        break;
    case 3:
        measurement->control = "Motor";
        start = ui->doubleSpinBoxMotorStart->value();
        end = ui->doubleSpinBoxMotorEnd->value();
        step = ui->doubleSpinBoxMotorStep->value();
        break;
    }
    if( (ui->controlGroup->checkedId() != 0 ) && ((start > end && step > 0 ) || fabs(end-start) < fabs(step) || step == 0 )){
                QMessageBox msgBox;
                msgBox.setText(tr("Wrong Minimum, Maximum and Step"));
                msgBox.exec();
                return ;
    }

    measurement->name = ui->edit_name->text();
    measurement->description = ui->plainTextEdit->toPlainText();
    measurement->matrix = (matrix_t) ui->combo_matrix->currentText().toInt();
    measurement->dvm_time = ui->combo_dvm_time->itemData(ui->combo_dvm_time->currentIndex()).toInt();
    measurement->average_number = ui->spinBoxAverage->value();
    measurement->settling_time = ui->doubleSpinBoxSettling->value();
//    measurement->start = ui->doubleSpinBoxStart->value();
//    measurement->end = ui->doubleSpinBoxEnd->value();
//    measurement->step = ui->doubleSpinBoxStep->value();
    measurement->end = end;
    measurement->step = step;

    measurement->n = ui->spinBoxN->value();

    measurement->start_hash["Alpha"]  = ui->doubleSpinBoxAlphaStart->value();
    measurement->start_hash["Beta"] = ui->doubleSpinBoxBetaStart->value();
    measurement->start_hash["Motor"] = ui->doubleSpinBoxMotorStart->value();

    ZeroModel *zero = list->at(ui->combo_zero->currentIndex());
    for (int k=0; k < NFORCES; k++){
        measurement->zero[k] = zero->force[k].at(0);
    }
    measurement->zero_name = zero->name;
    QDialog::accept();
}

void MeasurementsPreferences::maxminstep_enabled(int id){
    switch (id){
    case NONE:
        ui->spinBoxN->setEnabled(true);
        ui->doubleSpinBoxAlphaEnd->setEnabled(false);
        ui->doubleSpinBoxAlphaStep->setEnabled(false);
        ui->doubleSpinBoxBetaEnd->setEnabled(false);
        ui->doubleSpinBoxBetaStep->setEnabled(false);
        ui->doubleSpinBoxMotorEnd->setEnabled(false);
        ui->doubleSpinBoxMotorStep->setEnabled(false);
        return;
    case ALPHA:
        ui->spinBoxN->setEnabled(false);
        ui->doubleSpinBoxAlphaEnd->setEnabled(true);
        ui->doubleSpinBoxAlphaStep->setEnabled(true);
        ui->doubleSpinBoxBetaEnd->setEnabled(false);
        ui->doubleSpinBoxBetaStep->setEnabled(false);
        ui->doubleSpinBoxMotorEnd->setEnabled(false);
        ui->doubleSpinBoxMotorStep->setEnabled(false);
        return;
    case BETA:
        ui->spinBoxN->setEnabled(false);
        ui->doubleSpinBoxAlphaEnd->setEnabled(false);
        ui->doubleSpinBoxAlphaStep->setEnabled(false);
        ui->doubleSpinBoxBetaEnd->setEnabled(true);
        ui->doubleSpinBoxBetaStep->setEnabled(true);
        ui->doubleSpinBoxMotorEnd->setEnabled(false);
        ui->doubleSpinBoxMotorStep->setEnabled(false);
        return;
    case MOTOR:
        ui->spinBoxN->setEnabled(false);
        ui->doubleSpinBoxAlphaEnd->setEnabled(false);
        ui->doubleSpinBoxAlphaStep->setEnabled(false);
        ui->doubleSpinBoxBetaEnd->setEnabled(false);
        ui->doubleSpinBoxBetaStep->setEnabled(false);
        ui->doubleSpinBoxMotorEnd->setEnabled(true);
        ui->doubleSpinBoxMotorStep->setEnabled(true);
        return;
    }
}

void MeasurementsPreferences::on_combo_matrix_currentIndexChanged(int index){
    proxyfilter->setFilterFixedString(QString::number(index));
}
