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

    ui->doubleSpinBoxAlpha->setRange(-ANGLEMAX_ALPHA,ANGLEMAX_ALPHA);
    ui->doubleSpinBoxAlpha->setSingleStep(DEFAULT_ALPHA_STEP);
    ui->doubleSpinBoxBeta->setRange(-ANGLEMAX_BETA,ANGLEMAX_BETA);
    ui->doubleSpinBoxBeta->setSingleStep(DEFAULT_BETA_STEP);
    ui->doubleSpinBoxMotor->setRange(DEFAULT_MOTOR_MIN,DEFAULT_MOTOR_MAX);
    ui->doubleSpinBoxMotor->setSingleStep(DEFAULT_MOTOR_STEP);

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
    if (ui->controlGroup->checkedId() !=NONE ){
        double start,end,step;

        start = ui->doubleSpinBoxStart->value();
        end = ui->doubleSpinBoxEnd->value();
        step = ui->doubleSpinBoxStep->value();

        if( (start > end && step > 0 ) || fabs(end-start) < fabs(step) || step == 0  ){
            QMessageBox msgBox;
            msgBox.setText(tr("Wrong Minimum, Maximum and Step"));
            msgBox.exec();
            return ;
        }
    }

    measurement->name = ui->edit_name->text();
    measurement->description = ui->plainTextEdit->toPlainText();
    measurement->matrix = (matrix_t) ui->combo_matrix->currentText().toInt();
    measurement->dvm_time = ui->combo_dvm_time->itemData(ui->combo_dvm_time->currentIndex()).toInt();
    measurement->average_number = ui->spinBoxAverage->value();
    measurement->settling_time = ui->doubleSpinBoxSettling->value();
    measurement->start = ui->doubleSpinBoxStart->value();
    measurement->end = ui->doubleSpinBoxEnd->value();
    measurement->step = ui->doubleSpinBoxStep->value();
    measurement->control_type = (control_types_t) ui->controlGroup->checkedId();
    measurement->n = ui->spinBoxN->value();

    measurement->set_alpha = ui->doubleSpinBoxAlpha->value();
    measurement->set_beta = ui->doubleSpinBoxBeta->value();
    measurement->set_motor = ui->doubleSpinBoxMotor->value();

    ZeroModel *zero = list->at(ui->combo_zero->currentIndex());
    for (int k=0; k < NFORCES; k++){
        measurement->zero[k] = zero->force[k].at(0);
    }
    measurement->zero_name = zero->name;
    QDialog::accept();
}

void MeasurementsPreferences::maxminstep_enabled(int id){
    double start = 0 ,end = 0,step;
    switch (id){
    case NONE:  ui->doubleSpinBoxStart->setEnabled(false);
        ui->doubleSpinBoxEnd->setEnabled(false);
        ui->doubleSpinBoxStep->setEnabled(false);
        ui->spinBoxN->setEnabled(true);
        ui->doubleSpinBoxAlpha->setEnabled(true);
        ui->doubleSpinBoxBeta->setEnabled(true);
        ui->doubleSpinBoxMotor->setEnabled(true);
        return;
    case ALPHA:
        ui->doubleSpinBoxAlpha->setEnabled(false);
        ui->doubleSpinBoxBeta->setEnabled(true);
        ui->doubleSpinBoxMotor->setEnabled(true);
        start = - ANGLEMAX_ALPHA;
        end = ANGLEMAX_ALPHA;
        step = DEFAULT_ALPHA_STEP;
        break;
    case BETA:
        ui->doubleSpinBoxAlpha->setEnabled(true);
        ui->doubleSpinBoxBeta->setEnabled(false);
        ui->doubleSpinBoxMotor->setEnabled(true);
        start = -ANGLEMAX_BETA;
        end = ANGLEMAX_BETA;
        step = DEFAULT_BETA_STEP;
        break;
    case MOTOR:
        ui->doubleSpinBoxAlpha->setEnabled(true);
        ui->doubleSpinBoxBeta->setEnabled(true);
        ui->doubleSpinBoxMotor->setEnabled(false);
        start = DEFAULT_MOTOR_MIN;
        end = DEFAULT_MOTOR_MAX;
        step = DEFAULT_MOTOR_STEP;
        break;
    }
    ui->spinBoxN->setEnabled(false);
    ui->doubleSpinBoxStart->setEnabled(true);
    ui->doubleSpinBoxEnd->setEnabled(true);
    ui->doubleSpinBoxStep->setEnabled(true);
    ui->doubleSpinBoxStart->setRange(start,end);
    ui->doubleSpinBoxStart->setSingleStep(step);
    ui->doubleSpinBoxEnd->setRange(start,end);
    ui->doubleSpinBoxEnd->setSingleStep(step);
    ui->doubleSpinBoxStep->setRange(start,end);
    ui->doubleSpinBoxStep->setSingleStep(step);
    ui->doubleSpinBoxStart->setValue(start);
    ui->doubleSpinBoxEnd->setValue(end);
    ui->doubleSpinBoxStep->setValue(step);
}

void MeasurementsPreferences::on_combo_matrix_currentIndexChanged(int index){
    proxyfilter->setFilterFixedString(QString::number(index));
}
