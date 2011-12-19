#include "measurementspreferences.h"
#include "ui_measurementspreferences.h"

#include "QDebug"
#include "QMessageBox"

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
    ui->doubleSpinBoxWind->setRange(DEFAULT_WIND_MIN,DEFAULT_WIND_MAX);
    ui->doubleSpinBoxWind->setSingleStep(DEFAULT_WIND_STEP);

    ui->controlGroup->setId(ui->radioButtonNone,NONE);
    ui->controlGroup->setId(ui->radioButtonAlpha,ALPHA);
    ui->controlGroup->setId(ui->radioButtonBeta,BETA);
    ui->controlGroup->setId(ui->radioButtonWind,WIND);

    connect(ui->controlGroup, SIGNAL(buttonClicked(int)),this,SLOT(maxminstep_enabled(int)));

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
        double min,max,step;

        min = ui->doubleSpinBoxMin->value();
        max = ui->doubleSpinBoxMax->value();
        step = ui->doubleSpinBoxStep->value();

        if( min >= max || max-min < step || step == 0  ){
            QMessageBox msgBox;
            msgBox.setText(tr("Wrong Minimum, Maximum and Step"));
            msgBox.exec();
            return ;
        }
    }

//    emit MeasurementsPreferences::accept();
    measurement->name = ui->edit_name->text();
    measurement->description = ui->plainTextEdit->toPlainText();
    measurement->matrix = (matrix_t) ui->combo_matrix->currentText().toInt();
    measurement->dvm_time = ui->combo_dvm_time->itemData(ui->combo_dvm_time->currentIndex()).toInt();
    measurement->average_number = ui->spinBoxAverage->value();
    measurement->settling_time = ui->doubleSpinBoxSettling->value();
    measurement->min = ui->doubleSpinBoxMin->value();
    measurement->max = ui->doubleSpinBoxMax->value();
    measurement->step = ui->doubleSpinBoxStep->value();
    measurement->control_type = (control_types_t) ui->controlGroup->checkedId();
    measurement->n = ui->spinBoxN->value();

    measurement->set_alpha = ui->doubleSpinBoxAlpha->value();
    measurement->set_beta = ui->doubleSpinBoxBeta->value();
    measurement->set_wind = ui->doubleSpinBoxWind->value();

    measurement->zero = list->at(ui->combo_zero->currentIndex());
    measurement->zero_id = measurement->zero->id;
    QDialog::accept();
}

void MeasurementsPreferences::maxminstep_enabled(int id){
    double min = 0 ,max = 0,step;
    switch (id){
    case NONE:  ui->doubleSpinBoxMin->setEnabled(false);
        ui->doubleSpinBoxMax->setEnabled(false);
        ui->doubleSpinBoxStep->setEnabled(false);
        //                ui->doubleSpinBoxSettling->setEnabled(false);
        ui->spinBoxN->setEnabled(true);
        ui->doubleSpinBoxAlpha->setEnabled(true);
        ui->doubleSpinBoxBeta->setEnabled(true);
        ui->doubleSpinBoxWind->setEnabled(true);
        return;
    case ALPHA:
        ui->doubleSpinBoxAlpha->setEnabled(false);
        ui->doubleSpinBoxBeta->setEnabled(true);
        ui->doubleSpinBoxWind->setEnabled(true);
        min = - ANGLEMAX_ALPHA;
        max = ANGLEMAX_ALPHA;
        step = DEFAULT_ALPHA_STEP;
        break;
    case BETA:
        ui->doubleSpinBoxAlpha->setEnabled(true);
        ui->doubleSpinBoxBeta->setEnabled(false);
        ui->doubleSpinBoxWind->setEnabled(true);
        min = -ANGLEMAX_BETA;
        max = ANGLEMAX_BETA;
        step = DEFAULT_BETA_STEP;
        break;
    case WIND:
        ui->doubleSpinBoxAlpha->setEnabled(true);
        ui->doubleSpinBoxBeta->setEnabled(true);
        ui->doubleSpinBoxWind->setEnabled(false);
        min = DEFAULT_WIND_MIN;
        max = DEFAULT_WIND_MAX;
        step = DEFAULT_WIND_STEP;
        break;
    }
    ui->spinBoxN->setEnabled(false);
    ui->doubleSpinBoxMin->setEnabled(true);
    ui->doubleSpinBoxMax->setEnabled(true);
    ui->doubleSpinBoxStep->setEnabled(true);
    //    ui->doubleSpinBoxSettling->setEnabled(true);
    ui->doubleSpinBoxMin->setRange(min,max);
    ui->doubleSpinBoxMin->setSingleStep(step);
    ui->doubleSpinBoxMax->setRange(min,max);
    ui->doubleSpinBoxMax->setSingleStep(step);
    ui->doubleSpinBoxStep->setRange(min,max);
    ui->doubleSpinBoxStep->setSingleStep(step);
    ui->doubleSpinBoxMin->setValue(min);
    ui->doubleSpinBoxMax->setValue(max);
    ui->doubleSpinBoxStep->setValue(step);

}

void MeasurementsPreferences::on_combo_matrix_currentIndexChanged(int index){
    proxyfilter->setFilterFixedString(QString::number(index));
}
