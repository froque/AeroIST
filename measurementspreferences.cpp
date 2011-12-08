#include "measurementspreferences.h"
#include "ui_measurementspreferences.h"

#include "QDebug"
#include "QMessageBox"

MeasurementsPreferences::MeasurementsPreferences(MeasurementsModel *measurement, ZeroList *list, QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementsPreferences),
    measurement(measurement),
    list(list)
{
    ui->setupUi(this);

    proxyfilter = new QSortFilterProxyModel(this);
    proxyfilter->setSourceModel(list);
    proxyfilter->setFilterRole(Qt::UserRole);
    proxyfilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->combo_zero->setModel(proxyfilter);

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

    ui->doubleSpinBoxSettling->setValue(settings->value("default_settling_time").toDouble());
    ui->controlGroup->setId(ui->radioButtonNone,NONE);
    ui->controlGroup->setId(ui->radioButtonAlpha,ALPHA);
    ui->controlGroup->setId(ui->radioButtonBeta,BETA);
    ui->controlGroup->setId(ui->radioButtonWind,WIND);

    connect(ui->controlGroup, SIGNAL(buttonClicked(int)),this,SLOT(maxminstep_enabled(int)));

    adjustSize();

}


MeasurementsPreferences::~MeasurementsPreferences()
{
    delete ui;
}

void MeasurementsPreferences::on_buttonBox_accepted()
{
    measurement->name = ui->edit_name->text();
    measurement->matrix = (matrix_t) ui->combo_matrix->currentText().toInt();
    measurement->dvm_time = ui->combo_dvm_time->itemData(ui->combo_dvm_time->currentIndex()).toInt();
    measurement->average_number = ui->spinBoxAverage->value();
    measurement->settling_time = ui->doubleSpinBoxSettling->value();
    measurement->min = ui->doubleSpinBoxMin->value();
    measurement->max = ui->doubleSpinBoxMax->value();
    measurement->step = ui->doubleSpinBoxStep->value();
    measurement->control_type = (control_types_t) ui->controlGroup->checkedId();
    measurement->n = ui->spinBoxN->value();

    measurement->zero = list->at(ui->combo_zero->currentIndex());

}

void MeasurementsPreferences::test_input(){
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

    emit MeasurementsPreferences::accept();
}

void MeasurementsPreferences::maxminstep_enabled(int id){
    double min = 0 ,max = 0,step;
    switch (id){
    case NONE:  ui->doubleSpinBoxMin->setEnabled(false);
                ui->doubleSpinBoxMax->setEnabled(false);
                ui->doubleSpinBoxStep->setEnabled(false);
                ui->doubleSpinBoxSettling->setEnabled(false);
                ui->spinBoxN->setEnabled(true);
                return;
    case ALPHA:
                min = - ANGLEMAX_ALPHA;
                max = ANGLEMAX_ALPHA;
                step = DEFAULT_ALPHA_STEP;
                break;
    case BETA:
                min = -ANGLEMAX_BETA;
                max = ANGLEMAX_ALPHA;
                step = DEFAULT_BETA_STEP;
                break;
    case WIND:
                min = DEFAULT_WIND_MIN;
                max = DEFAULT_WIND_MAX;
                step = DEFAULT_WIND_STEP;
                break;
    }

    ui->doubleSpinBoxMin->setEnabled(true);
    ui->doubleSpinBoxMax->setEnabled(true);
    ui->doubleSpinBoxStep->setEnabled(true);
    ui->doubleSpinBoxSettling->setEnabled(true);
    ui->doubleSpinBoxMin->setMinimum(min);
    ui->doubleSpinBoxMin->setMaximum(max);
    ui->doubleSpinBoxMin->setSingleStep(step);
    ui->doubleSpinBoxMax->setMinimum(min);
    ui->doubleSpinBoxMax->setMaximum(max);
    ui->doubleSpinBoxMax->setSingleStep(step);
    ui->doubleSpinBoxStep->setMinimum(min);
    ui->doubleSpinBoxStep->setMaximum(max);
    ui->doubleSpinBoxStep->setSingleStep(step);
    ui->doubleSpinBoxMin->setValue(min);
    ui->doubleSpinBoxMax->setValue(max);
    ui->doubleSpinBoxStep->setValue(step);
    ui->spinBoxN->setEnabled(false);
}

void MeasurementsPreferences::on_combo_matrix_currentIndexChanged(int index){
    proxyfilter->setFilterFixedString(QString::number(index));
}
