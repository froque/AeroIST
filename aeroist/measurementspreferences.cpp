#include "measurementspreferences.h"
#include "ui_measurementspreferences.h"

#include "QMessageBox"
#include <cmath>
#include <QSettings>

#define COL_RADIO 1
#define COL_START 2
#define COL_END   3
#define COL_STEP  4


MeasurementsPreferences::MeasurementsPreferences(MeasurementsModel *measurement, ReferenceList *list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementsPreferences),
    measurement(measurement),
    list(list)
{
    ui->setupUi(this);

    QSettings settings;

    ui->combo_zero->setModel(list);

    ui->spinBoxMeasuresIteration->setValue(settings.value(SETTINGS_DEFAULT_MEASURES_ITERATION).toInt());
    ui->doubleSpinBoxSettling->setValue(settings.value(SETTINGS_DEFAULT_SETTLING_TIME).toDouble());

    // adds widgets to tabwidget for each configurable variable
    foreach (VariableModel *var, measurement->variables) {
        if(var->measurement_is_configurable()){
            ui->tabWidget->addTab(var->measurement_get_widget(),var->meta->get_general_name_tr());
        }
    }

    QGridLayout *layout = new QGridLayout;
    QLabel *label;
    QRadioButton *radio;
    QDoubleSpinBox *spin;

    int row = 0;

    group = new QButtonGroup(this);
    radio_none = new QRadioButton(tr("None"));
    radio_none->setChecked(true);
    layout->addWidget(radio_none,row,COL_RADIO);
    group->addButton(radio_none);
    label = new QLabel(trUtf8("Nº of iterations"));
    layout->addWidget(label,row,COL_START);          // not really a start spinbox, but just to be pretty
    spin_iterations = new QSpinBox;
    spin_iterations->setRange(0,1000);
    layout->addWidget(spin_iterations,row,COL_END);

    // count the number of controlables
    int num_controls = 0;
    foreach (VariableModel *var, measurement->variables) {
        if (var->meta->is_controlable()){
            num_controls += var->meta->get_num();
        }
    }
    if (num_controls > 0){
        row++;
        label = new QLabel (tr("Start"));
        layout->addWidget(label,row,COL_START);
        label = new QLabel (tr("End"));
        layout->addWidget(label,row,COL_END);
        label = new QLabel (tr("Step"));
        layout->addWidget(label,row,COL_STEP);
        row++;

        foreach (VariableModel *var, measurement->variables) {
            if (var->meta->is_controlable()){
                for (int k=0; k< var->meta->get_num(); k++){
                    radio = new QRadioButton(var->meta->get_name_tr(k).append(" (").append(var->meta->get_units(k)).append(")"));
                    radio->setObjectName(var->meta->get_name(k));
                    list_radio.append(radio);
                    layout->addWidget(radio, row, COL_RADIO);
                    group->addButton(radio,row);
                    spin = new QDoubleSpinBox;
                    spin->setRange(var->meta->get_lower_bound(k),var->meta->get_upper_bound(k));
                    spin->setSingleStep(var->meta->get_default_step(k));
                    spin->setValue(var->meta->get_default_start(k));
                    list_start.append(spin);
                    layout->addWidget(spin,row,COL_START);
                    spin = new QDoubleSpinBox;
                    spin->setRange(var->meta->get_lower_bound(k),var->meta->get_upper_bound(k));
                    spin->setSingleStep(var->meta->get_default_step(k));
                    spin->setValue(var->meta->get_upper_bound(k));
                    list_end.append(spin);
                    layout->addWidget(spin,row,COL_END);
                    spin = new QDoubleSpinBox;
                    spin->setRange(var->meta->get_smaller_step(k),var->meta->get_upper_bound(k) - var->meta->get_lower_bound(k));
                    spin->setSingleStep(var->meta->get_default_step(k));
                    spin->setValue(var->meta->get_default_step(k));
                    list_step.append(spin);
                    layout->addWidget(spin,row,COL_STEP);
                    row++;
                }
            }
        }
    }

    ui->tab_control->setLayout(layout);

    connect(group, SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(maxminstep_enabled(QAbstractButton*)));
    ui->edit_name->setFocus();
    maxminstep_enabled(radio_none); // to disable other spinboxs
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
        msgBox.setText(tr("No References. Create one first"));
        msgBox.exec();
        return ;
    }

    double start = 0, end = 0,step = 0;
    // what is the desired control?
    if (group->checkedButton() == radio_none){
        measurement->control = "";
    } else {
        measurement->control = group->checkedButton()->objectName();
        int k = list_radio.indexOf( qobject_cast<QRadioButton*>(group->checkedButton()));
        start = list_start.value(k)->value();
        end = list_end.value(k)->value();
        step = list_step.value(k)->value();
        if( start > end ){
            step = -step;
        }
    }

    ReferenceModel *ref = list->at(ui->combo_zero->currentIndex());
    foreach (VariableModel *ref_var, ref->variables) {
        foreach (VariableModel *var, measurement->variables) {
            if (var->meta->get_general_name() == ref_var->meta->get_general_name()){
                if(var->measurement_accept_config(ref_var) == false){
                    return;
                }
            }
        }
    }

    measurement->name = ui->edit_name->text();
    measurement->description = ui->plainTextEdit->toPlainText();
    measurement->measures_per_iteration = ui->spinBoxMeasuresIteration->value();
    measurement->settling_time = ui->doubleSpinBoxSettling->value();

    measurement->end = end;
    measurement->step = step;

    measurement->iterations = spin_iterations->value();

    foreach (VariableModel *var, measurement->variables) {
        if (var->meta->is_controlable()){
            var->start = QVector<double>(var->meta->get_num());
            for (int l=0; l<list_start.size(); l++){
                for (int k=0; k< var->meta->get_num(); k++){
                    if(var->meta->get_name(k) == list_radio.value(l)->objectName()){
                        var->start[k] = list_start.value(l)->value();
                    }
                }
            }
        }
    }

    ref = list->at(ui->combo_zero->currentIndex());
    QVector<double> vector,avg;
    foreach (VariableModel *ref_var, ref->variables) {
        foreach (VariableModel *var, measurement->variables) {
            if(var->meta->has_zero()){
                if (var->meta->get_general_name() == ref_var->meta->get_general_name()){
                    for(int k=0; k< ref_var->meta->get_num(); k++){
                        avg = ref_var->data->get_vector(k);
                        double total = 0.0;
                        int size = avg.size();
                        for (int n=0; n < size; n++){
                            total += avg.at(n);
                        }
                        total = total / size;
                        vector.append(total);
                    }
                    var->data->set_zero(vector);
                }
            }
        }
    }
    measurement->ref_name = ref->name;
    QDialog::accept();
}

void MeasurementsPreferences::maxminstep_enabled(QAbstractButton* button) {
    QDoubleSpinBox *spin;
    foreach (spin, list_end) {
        spin->setEnabled(false);
    }
    foreach (spin, list_step) {
        spin->setEnabled(false);
    }
    if (button == radio_none){
        spin_iterations->setEnabled(true);
    } else {
        spin_iterations->setEnabled(false);
        int row = list_radio.indexOf( qobject_cast<QRadioButton*>(button));
        list_end.value(row)->setEnabled(true);
        list_step.value(row)->setEnabled(true);
    }
}
