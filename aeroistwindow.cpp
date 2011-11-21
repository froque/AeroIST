#include "aeroistwindow.h"
#include "ui_aeroistwindow.h"

#include <QMetaType>
#include <QFile>
#include <QFileDialog>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_renderer.h>
//#include "timescaledraw.h"
#include <QMessageBox>
#include "measurementspreferences.h"
#include "measurementdetails.h"
#ifdef DEBUG
#include <QDebug>
#endif //DEBUG


AeroISTWindow::AeroISTWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AeroISTWindow)
{
    ui->setupUi(this);

    // Set some standard (freedesktop only) Icons
    ui->actionNew_Measure->setIcon(QIcon::fromTheme("document-new"));
    ui->actionDelete_Measure->setIcon(QIcon::fromTheme("edit-delete"));
    ui->actionView_Measure_details->setIcon(QIcon::fromTheme("document-properties"));
//    ui->actionClear_Project->setIcon(QIcon::fromTheme("edit-clear"));
    ui->actionLoad_Project->setIcon(QIcon::fromTheme("document-open"));
    ui->actionSave_Project->setIcon(QIcon::fromTheme("document-save-as"));
    ui->actionPreferences->setIcon(QIcon::fromTheme(""));

    // Set the list model
    measure_list = new MeasureList();
    ui->listView->setModel(measure_list);

    zero_list = new MeasureList();
    ui->listViewZero->setModel(zero_list);

    // listview personalization. It couldn't be done from the .ui file
    QItemSelectionModel *selection = ui->listView->selectionModel();
    connect(selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(selectionChanged(QModelIndex,QModelIndex)));
    ui->listView->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->listView->insertAction(0,ui->actionDelete_Measure);
    ui->listView->insertAction(0,ui->actionView_Measure_details);

    selection = ui->listViewZero->selectionModel();
//    connect(selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(selectionChanged(QModelIndex,QModelIndex)));
    ui->listViewZero->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->listViewZero->insertAction(0,ui->actionDelete_Zero);
    ui->listViewZero->insertAction(0,ui->actionView_Zero_details);

    // load settings from .ini file
    load_settings();
    preferences = new Preferences(settings,this);

#ifdef INITIAL_LIST
    MeasurementsModel *measurement;
    MeasurementsModel *zero;
    for (int k=0;k<6;k++){

        measurement = new MeasurementsModel(this);
        measurement->name = QString("Measure n %1%1%1").arg(k);
        measurement->matrix = MIDDLE;
        measurement->dvm_time = 4;
        measurement->average_number = 2;
        measurement->isZero = false;

        zero = new MeasurementsModel(this);
        zero->isZero=true;
        zero->name = QString(tr("Zero %1%1%1")).arg(k);
        zero->matrix = MIDDLE;
        zero->dvm_time = 4;
        zero->average_number = 1;
        zero->control_type=NONE;
        switch (k%4){
        case 0:
            measurement->control_type = NONE;
            break;
        case 1:
            measurement->control_type = ALPHA;
            break;
        case 2:
            measurement->control_type = BETA;
            break;
        case 3:
            measurement->control_type = WIND;
            break;
        }
        measurement->min = 100 ;//* (k%4 +1);
        measurement->max = 200 ;//* (k%4 +1);
        measurement->step = 21 ;// * (k%4 +1);
        measurement->settling_time = 0 ;
        measure_list->newMeasure(measurement);
        zero_list->newMeasure(zero);
        ui->listView->setCurrentIndex(measure_list->index(measure_list->rowCount()-1,0));
    }
#endif //INITIAL_LIST

    qRegisterMetaType<measure>("measure");
//    ThreadRunning = false;
    thread_status = STOPPED;
}

AeroISTWindow::~AeroISTWindow()
{
//    thread->wait();
    save_settings();
    delete preferences;
    delete settings;
    delete ui;
    delete measure_list;
    delete zero_list;
}

// Starts a new measurement.
void AeroISTWindow::on_ThreadButton_clicked()
{
    if (thread_status == STOPPED){
        QModelIndex index = ui->listView->currentIndex();
        if (index.isValid() == false){
            QMessageBox message;
            message.setText("index not valid");
            message.exec();
            return;
        }
        measurementThread = measure_list->at(index.row());         // get the index
        if (measurementThread->rowCount(QModelIndex()) !=0){
            QMessageBox message;
            message.setText("Measure is not empty");
            message.exec();
            return;
        }
        producer = new MeasureThread(measurementThread);           // start new producer, with the measurementModel metadata
        producer->moveToThread(&producerThread);
        thread_status = MEASURE_RUNNING;
        start_loop( measurementThread,producer,ui->ThreadButton,SLOT(click()));

        QString text("Stop ");
        text.append( measurementThread->name);
        ui->ThreadButton->setText(text);
        return;
    }
    if (thread_status == MEASURE_RUNNING){
        stop_loop( measurementThread,producer,ui->ThreadButton,SLOT(click()));
        ui->ThreadButton->setText("Start");
        producer->deleteLater();
        thread_status = STOPPED;
        return;
    }
}

void AeroISTWindow::start_loop( MeasurementsModel * measurement, MeasureThread *measureThread, const QObject *receiver, const char * slot ){
    connect(measureThread->thread(), SIGNAL(started()),measureThread,SLOT(start_timer()));                // 1 -just signal to start timer, on thread start
    connect(measureThread->thread(), SIGNAL(started()), measureThread ,SLOT(produce()));                  // 2 -when thread start, it bootstraps the loop
    connect(measurement,SIGNAL(produceMeasure()),measureThread,SLOT(produce()));               // 3 -produce more, the other half of the loop
    connect(measureThread,SIGNAL(MeasureDone(measure)),measurement,SLOT(GetMeasure(measure))); // 4- pass the measurement, half the loop
    connect(measureThread->thread(),SIGNAL(finished()),receiver,slot);
    measureThread->thread()->start();
}
void AeroISTWindow::stop_loop( MeasurementsModel * measurement, MeasureThread *measureThread, const QObject *receiver, const char * slot ){
    measureThread->thread()->quit();
    disconnect(measureThread->thread(), SIGNAL(started()),measureThread,SLOT(start_timer()));                // 1 -just signal to start timer, on thread start
    disconnect(measureThread->thread(), SIGNAL(started()), measureThread ,SLOT(produce()));                  // 2 -when thread start, it bootstraps the loop
    disconnect(measurement,SIGNAL(produceMeasure()),measureThread,SLOT(produce()));               // 3 -produce more, the other half of the loop
    disconnect(measureThread,SIGNAL(MeasureDone(measure)),measurement,SLOT(GetMeasure(measure))); // 4- pass the measurement, half the loop
    disconnect(measureThread->thread(),SIGNAL(finished()),receiver,slot);
}

// Clear Plots
void AeroISTWindow::on_ClearButton_clicked()
{
    ui->qwtPlot->detachItems(QwtPlotItem::Rtti_PlotItem,true);
    ui->qwtPlot->replot();
}

// Plot a graph based on selected measurement
void AeroISTWindow::on_GraphButton_clicked(){
    QwtPlotCurve *curve1 = new QwtPlotCurve();

    int row =  ui->listView->currentIndex().row();
    if (row < 0 || row > measure_list->rowCount()){
        return;
    }

    MeasurementsModel *measurement;
    measurement = measure_list->at(row);

    curve1->setSamples(measurement->tempo.data(),measurement->force[0].data(),measurement->tempo.size());
    curve1->attach(ui->qwtPlot);
    ui->qwtPlot->replot();
}

// export selected measurement to .csv
void AeroISTWindow::on_actionExport_to_csv_triggered()
{
    int row =  ui->listView->currentIndex().row();
    if (row < 0 || row > measure_list->rowCount()){
        return;
    }
    MeasurementsModel *measurement;
    measurement = measure_list->at(row);

    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, tr("Export mesurements"), ".", "");

    QFile data(fileName);
    if (data.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&data);       
        measurement->save_csv(&out,true);
    }
}

// Save current plot to file based on extension
void AeroISTWindow::on_actionExportPlot_triggered()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, tr("Export plot"), ".", "" );

    QwtPlotRenderer* renderer = new QwtPlotRenderer();
    renderer->renderDocument(ui->qwtPlot,fileName,QSize(297,210),300);  //A4 size, 300 DPI
}

void AeroISTWindow::on_actionNew_Measure_triggered()
{
    MeasurementsModel *measurement;
    measurement = new MeasurementsModel();
    measurement->isZero = false;
    MeasurementsPreferences *meas_prefs = new MeasurementsPreferences( measurement, zero_list, settings , this);
    if (meas_prefs->exec() == 0 ){
        delete measurement;
        delete meas_prefs;
        return ;
    }
    measure_list->newMeasure(measurement);
    ui->listView->setCurrentIndex(measure_list->index(measure_list->rowCount()-1,0));
    delete meas_prefs;
}


void AeroISTWindow::on_actionDelete_Measure_triggered()
{
    QModelIndex index = ui->listView->currentIndex();

    if (measure_list->at(index) == measurementThread && thread_status == MEASURE_RUNNING){
        QMessageBox msgBox;
        msgBox.setText("Measuring is being done. Stop it to delete");
        msgBox.exec();
        return;
    }

    // if the to be deleted model is shown in the table, unset
    if (measure_list->at(index) == ui->tableView->model()){
        ui->tableView->setModel(NULL);
        ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab),"Table");
    }
    measure_list->deleteMeasure(index);
    if (index.isValid() && index.row() < measure_list->rowCount()){
        qDebug() << index.row();
        ui->listView->setCurrentIndex(index);
    }
}

void AeroISTWindow::on_actionSave_Project_triggered()
{
//    if (producerThread.isRunning()){
//    if (ThreadRunning == true){
    if (thread_status != STOPPED){
        QMessageBox msgBox;
        msgBox.setText("Measuring is being done. Stop it to save to disk");
        msgBox.exec();
        return;
    }
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, tr("Save Project"), ".", "");
    measure_list->save(fileName);
}


void AeroISTWindow::on_actionLoad_Project_triggered()
{
    if ( measure_list->rowCount() != 0){
        QMessageBox msgBox;
        msgBox.setText("There is data in the project");
        msgBox.exec();
        return;
    }
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Load Project"), ".", "");

    measure_list->load(fileName);
}


void AeroISTWindow::on_actionClear_Project_triggered()
{
    measure_list->clear();
    zero_list->clear();
}


// Open preferences dialog
void AeroISTWindow::on_actionPreferences_triggered()
{
    preferences->exec();
}


void AeroISTWindow::selectionChanged(const QModelIndex &current ,const QModelIndex &previous){
    Q_UNUSED(previous);
    if (current.isValid()){
        ui->ThreadButton->setEnabled(true);
        ui->actionDelete_Measure->setEnabled(true);
        ui->actionView_Measure_details->setEnabled(true);
    } else {
        ui->ThreadButton->setEnabled(false);
        ui->actionDelete_Measure->setEnabled(false);        // BUG unity, fixme later
        ui->actionView_Measure_details->setEnabled(false);
    }
}


void AeroISTWindow::on_listView_activated(const QModelIndex &index)
{
    ui->tableView->setModel( measure_list->at(index.row()));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab),measure_list->at(index.row())->name);
}

void AeroISTWindow::load_settings(void){
    settings = new QSettings("IST", "AeroIST");

    restoreState(settings->value("gui/state").toByteArray());
    restoreGeometry(settings->value("gui/geometry").toByteArray());

    ui->splitterLists->restoreState(settings->value("gui/splitterLists/state").toByteArray());
    ui->splitterGlobal->restoreState(settings->value("gui/splitterGlobal/state").toByteArray());
}

void AeroISTWindow::save_settings(void){
    settings->setValue("gui/state",saveState());
    settings->setValue("gui/geometry",saveGeometry());

    settings->setValue("gui/splitterLists/state",ui->splitterLists->saveState());
    settings->setValue("gui/splitterGlobal/state",ui->splitterGlobal->saveState());
}

void AeroISTWindow::on_actionView_Measure_details_triggered()
{
    MeasurementsModel *measurement;
    QModelIndex index = ui->listView->currentIndex();
    if (index.isValid()){
        measurement = measure_list->at(index);

        MeasurementDetails *details = new MeasurementDetails(measurement,this);

        details->exec();
        delete details;
    }
}

void AeroISTWindow::on_actionToolbar_toggled(bool arg1)
{
    ui->toolBar->setVisible(arg1);
}

void AeroISTWindow::on_actionMeasure_List_toggled(bool checked)
{
    ui->listView->setVisible(checked);
}

void AeroISTWindow::on_actionDelete_Zero_triggered()
{
     QModelIndex index = ui->listViewZero->currentIndex();
     if (zero_list->at(index) == ZeroThread && thread_status == ZERO_RUNNING){
         QMessageBox msgBox;
         msgBox.setText("Measuring is being done. Stop it to delete");
         msgBox.exec();
         return;
     }

     // if the to be deleted model is in the table, unset
     if (zero_list->at(index) == ui->tableView->model()){
         ui->tableView->setModel(NULL);
         ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab),"Table");
     }
     zero_list->deleteMeasure(index);
     zero_list->deleteMeasure(index);
     if (index.isValid() && index.row() < zero_list->rowCount()){
         qDebug() << index.row();
         ui->listViewZero->setCurrentIndex(index);
     }
}


void AeroISTWindow::on_actionView_Zero_details_triggered()
{
    MeasurementsModel *zero;
    QModelIndex index = ui->listViewZero->currentIndex();
    if (index.isValid()){
        zero = zero_list->at(index);

        MeasurementDetails *details = new MeasurementDetails(zero,this);

        details->exec();
        delete details;
    }
}

void AeroISTWindow::on_actionZero_List_toggled(bool arg1)
{
    ui->listViewZero->setVisible(arg1);
}


void AeroISTWindow::on_listViewZero_activated(const QModelIndex &index)
{
    ui->tableView->setModel( zero_list->at(index.row()));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab),zero_list->at(index.row())->name);
}

void AeroISTWindow::on_actionNew_Zero_triggered()
{
    if (thread_status != STOPPED && thread_status != ZERO_RUNNING){
        QMessageBox message;
        message.setText("A measurement is being done");
        message.exec();
        return;
    }
    if (thread_status == STOPPED){
        MeasurementsPreferences *zero_prefs;
        ZeroThread = new MeasurementsModel;

        ZeroThread->isZero = true;
        zero_prefs = new MeasurementsPreferences(ZeroThread,NULL,settings,this);
        if (zero_prefs->exec() == 0){
            delete zero_prefs;
            delete ZeroThread;
            return;
        }
        delete zero_prefs;
        zero_list->newMeasure(ZeroThread);
        QModelIndex index = zero_list->index(zero_list->rowCount()-1,0);
        ui->listViewZero->setCurrentIndex(index);

        producer = new MeasureThread(ZeroThread);           // start new producer, with the measurementModel metadata
        producer->moveToThread(&producerThread);
        thread_status = ZERO_RUNNING;
        start_loop( ZeroThread,producer,this,SLOT(on_actionNew_Zero_triggered()));
        return;
    }
    if (thread_status == ZERO_RUNNING){
        stop_loop( ZeroThread,producer,this,SLOT(on_actionNew_Zero_triggered()));
        producer->deleteLater();
        thread_status = STOPPED;
        return;
    }
}

void AeroISTWindow::on_ZeroButton_clicked()
{
    if (thread_status == STOPPED){
        QModelIndex index = ui->listViewZero->currentIndex();
        if (index.isValid() == false){
            QMessageBox message;
            message.setText("index not valid");
            message.exec();
            return;
        }
        ZeroThread = zero_list->at(index.row());         // get the index
        if (ZeroThread->rowCount(QModelIndex()) !=0){
            QMessageBox message;
            message.setText("Measure is not empty");
            message.exec();
            return;
        }

        Zproducer = new MeasureThread(ZeroThread);           // start new producer, with the measurementModel metadata
        Zproducer->moveToThread(&producerThread);
        thread_status = ZERO_RUNNING;
        start_loop( ZeroThread,Zproducer,ui->ZeroButton,SLOT(click()));

        QString text("Stop ");
        text.append( ZeroThread->name);
        ui->ZeroButton->setText(text);
        return;
    }
    if (thread_status == ZERO_RUNNING){
        stop_loop( ZeroThread,Zproducer,ui->ZeroButton,SLOT(click()));
        ui->ZeroButton->setText("Start");
//        Zproducer->deleteLater();
//        delete Zproducer;
        thread_status = STOPPED;
        return;
    }
}

