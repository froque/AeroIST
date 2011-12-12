#include "aeroistwindow.h"
#include "ui_aeroistwindow.h"

#include <QMetaType>
#include <QFile>
#include <QFileDialog>
//#include <qwt/qwt_plot_curve.h>
//#include <qwt/qwt_plot_renderer.h>
//#include <qwt/qwt_legend.h>
#include <qwt_legend.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_renderer.h>
//#include "timescaledraw.h"
#include <QMessageBox>
#include <QtXml>
#include "measurementspreferences.h"
#include "zeropreferences.h"
#include "measurementdetails.h"
#include "zerodetails.h"
#include "curvenew.h"
#include "curvedelete.h"
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
    ui->actionClear_Project->setIcon(QIcon::fromTheme("edit-clear"));
    ui->actionLoad_Project->setIcon(QIcon::fromTheme("document-open"));
    ui->actionSave_Project->setIcon(QIcon::fromTheme("document-save-as"));
//    ui->actionPreferences->setIcon(QIcon::fromTheme(""));


    // Set the list model
    measure_list = new MeasureList();
    ui->listView->setModel(measure_list);

//    zero_list = new MeasureList();
    zero_list = new ZeroList();
    ui->listViewZero->setModel(zero_list);

    // listview personalization. It couldn't be done from the .ui file
    QItemSelectionModel *selection = ui->listView->selectionModel();
    connect(selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(selectionChanged(QModelIndex,QModelIndex)));
    ui->listView->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->listView->insertAction(0,ui->actionView_Measure_details);
    ui->listView->insertAction(0,ui->actionDelete_Measure);

    selection = ui->listViewZero->selectionModel();
    connect(selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(ZeroSelectionChanged(QModelIndex,QModelIndex)));
    ui->listViewZero->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->listViewZero->insertAction(0,ui->actionView_Zero_details);
    ui->listViewZero->insertAction(0,ui->actionDelete_Zero);

    // load settings from .ini file
    load_settings();
    preferences = new Preferences(settings,this);

#if INITIAL_LIST
    MeasurementsModel *measurement;
    ZeroModel *zero;
    for (int k=0;k<6;k++){

        measurement = new MeasurementsModel(this);
        measurement->name = QString(tr("Measure n %1%1%1")).arg(k);
        measurement->matrix = MIDDLE;
        measurement->dvm_time = 4;
        measurement->average_number = 2;

        measurement->n = k;
        zero = new ZeroModel(this);

        zero->name = QString(tr("Zero %1%1%1")).arg(k);
        zero->matrix = MIDDLE;
        zero->dvm_time = 4;
        zero->average_number = 1;
        zero->control_type=NONE;
        zero->n=1;

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
        measurement->step = 19 ;// * (k%4 +1);
        measurement->settling_time = 0 ;
        measure_list->newMeasure(measurement);
        zero_list->newMeasure(zero);
        measurement->zero = zero_list->at(k);
        ui->listView->setCurrentIndex(measure_list->index(measure_list->rowCount()-1,0));
//        ui->listViewZero->setCurrentIndex(zero_list->index(zero_list->rowCount()-1,0));

//        ui->ZeroButton->click();
    }
#endif //INITIAL_LIST

    qRegisterMetaType<measure>("measure");
    thread_status = STOPPED;

    m_thread = 0;
    m_test = 0;

    QwtLegend *legend = new QwtLegend;
    legend->setItemMode(QwtLegend::CheckableItem);
    ui->qwtPlot->insertLegend(legend);
    ui->doubleSpinBoxALpha->setRange(-ANGLEMAX_ALPHA,ANGLEMAX_ALPHA);
    ui->doubleSpinBoxALpha->setSingleStep(DEFAULT_ALPHA_STEP);
    ui->doubleSpinBoxALpha->setValue(0);
    ui->doubleSpinBoxBeta->setRange(-ANGLEMAX_BETA,ANGLEMAX_BETA);
    ui->doubleSpinBoxBeta->setSingleStep(DEFAULT_BETA_STEP);
    ui->doubleSpinBoxBeta->setValue(0);
    ui->doubleSpinBoxWind->setRange(DEFAULT_WIND_MIN,DEFAULT_WIND_MAX);
    ui->doubleSpinBoxWind->setSingleStep(DEFAULT_WIND_STEP);
    ui->doubleSpinBoxWind->setValue(0);


}

AeroISTWindow::~AeroISTWindow()
{
    save_settings();
    delete preferences;
    delete settings;
    delete ui;
    delete measure_list;
    delete zero_list;
}

void AeroISTWindow::on_ThreadButton_clicked(){

    if (thread_status != STOPPED && thread_status != MEASURE_RUNNING){
        message(tr("Thread busy"));
        return;
    }

    if (m_thread && m_test){
        return;
    }

    if(thread_status == STOPPED){
    QModelIndex index = ui->listView->currentIndex();
    if (index.isValid() == false){
        message(tr("index not valid"));
        return;
    }
    measurementThread = measure_list->at(index.row());         // get the index
    if (measurementThread->rowCount(QModelIndex()) !=0){
        message(tr("Measure is not empty"));
        return;
    }
    m_test = new MeasureThread(measurementThread);

    if (!m_thread)
        m_thread = new QThread(0);
    if (m_test->thread() != m_thread)
        m_test->moveToThread(m_thread);

    // start connects
    connect(m_thread, SIGNAL(started()), m_test, SLOT(produce()));

    // dump connect
    connect(m_test, SIGNAL(MeasureDone(measure)),measurementThread, SLOT(GetMeasure(measure)));

    // stop connects
    connect(ui->ThreadButton, SIGNAL(clicked()), m_test, SLOT(stop()));
//    connect(m_thread,SIGNAL(finished()),this,SLOT(cleanup()));
    connect(m_thread,SIGNAL(finished()),this,SLOT(ThreadButton_cleanup()));
//    connect(this,SIGNAL(set_alpha(double)),m_test,SLOT(set_alpha(double)));
//    connect(this,SIGNAL(set_beta(double)),m_test,SLOT(set_beta(double)));
//    connect(this,SIGNAL(set_wind(double)),m_test,SLOT(set_wind(double)));
    m_thread->start();
    thread_status = MEASURE_RUNNING;
    QString text(tr("Stop "));
    text.append( measurementThread->name);
    ui->ThreadButton->setText(text);
    return;
    }

}

void AeroISTWindow::ThreadButton_cleanup(){
    if (thread_status == MEASURE_RUNNING){
        ui->ThreadButton->setText(tr("Start"));
        thread_status = STOPPED;
        cleanup();
        return;
    }
}
void AeroISTWindow::cleanup(){
    if (m_thread->isRunning()){
        qWarning() << "thread is running";
        return;
    }
    delete m_test;
    m_test = 0;
    delete m_thread;
    m_thread = 0;
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

    MeasurementsPreferences *meas_prefs = new MeasurementsPreferences( measurement, zero_list, settings , this);
    if (meas_prefs->exec() == QDialog::Rejected ){
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
        message(tr("Measuring is being done. Stop it to delete"));
        return;
    }

    // if the to be deleted model is shown in the table, unset
    if (measure_list->at(index) == ui->tableView->model()){
        ui->tableView->setModel(NULL);
        ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab),tr("Table"));
    }
    measure_list->deleteMeasure(index);
    if (index.isValid() && index.row() < measure_list->rowCount()){
        qDebug() << index.row();
        ui->listView->setCurrentIndex(index);
    }
}

void AeroISTWindow::on_actionSave_Project_triggered(){
    if (thread_status != STOPPED){
        message(tr("Measuring is being done. Stop it to save to disk"));
        return;
    }
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, tr("Save Project"), ".", "");
//    measure_list->save(fileName);
    QDomDocument document;
    QDomElement root = document.createElement("Zero");
    document.appendChild(root);

    ZeroModel *zero;
    for (int k=0; k<zero_list->rowCount(); k++){
        zero = zero_list->at(k);
        QDomElement zero_element = document.createElement("Zero");
        document.appendChild(zero_element);

        QDomElement name = document.createElement("Name");
        name.appendChild(document.createTextNode(zero->name));
        zero_element.appendChild(name);

        QDomElement id = document.createElement("Id");
        id.appendChild(document.createTextNode(QString::number(k)));
        zero_element.appendChild(id);

        QDomElement description = document.createElement("Description");
        description.appendChild(document.createTextNode(zero->description));
        zero_element.appendChild(description);

        QDomElement dvm_time = document.createElement("DVM Time");
        dvm_time.appendChild(document.createTextNode(QString::number(zero->dvm_time)));
        zero_element.appendChild(dvm_time);

        QDomElement matrix = document.createElement("Matrix");
        matrix.appendChild(document.createTextNode(QString::number(zero->matrix)));
        zero_element.appendChild(matrix);

        QDomElement average_number = document.createElement("average_number");
        average_number.appendChild(document.createTextNode(QString::number(zero->average_number)));
        zero_element.appendChild(average_number);

        QDomElement set_alpha = document.createElement("Set Alpha");
        set_alpha.appendChild(document.createTextNode(QString::number(zero->set_beta)));
        zero_element.appendChild(set_alpha);

        QDomElement set_beta = document.createElement("Set Beta");
        set_beta.appendChild(document.createTextNode(QString::number(zero->set_beta)));
        zero_element.appendChild(set_beta);

        QDomElement set_wind = document.createElement("Set Wind");
        set_wind.appendChild(document.createTextNode(QString::number(zero->set_wind)));
        zero_element.appendChild(set_wind);

        QDomElement data = document.createElement("Data");
        zero_element.appendChild(data);

        QDomElement force;
        force = document.createElement("Force X");
        data.appendChild(force);
        for (int n=0; n < zero->rowCount(); n++){
            force.appendChild(document.createTextNode(QString::number(zero->force[0].at(n))));
        }
        force = document.createElement("Force Y");
        data.appendChild(force);
        for (int n=0; n < zero->rowCount(); n++){
            force.appendChild(document.createTextNode(QString::number(zero->force[0].at(n))));
        }
        force = document.createElement("Force Z");
        data.appendChild(force);
        for (int n=0; n < zero->rowCount(); n++){
            force.appendChild(document.createTextNode(QString::number(zero->force[0].at(n))));
        }
        force = document.createElement("Moment X");
        data.appendChild(force);
        for (int n=0; n < zero->rowCount(); n++){
            force.appendChild(document.createTextNode(QString::number(zero->force[0].at(n))));
        }
        force = document.createElement("Moment Y");
        data.appendChild(force);
        for (int n=0; n < zero->rowCount(); n++){
            force.appendChild(document.createTextNode(QString::number(zero->force[0].at(n))));
        }
        force = document.createElement("Moment Z");
        data.appendChild(force);
        for (int n=0; n < zero->rowCount(); n++){
            force.appendChild(document.createTextNode(QString::number(zero->force[0].at(n))));
        }
    }

    QFile file(fileName);
    if (file.open(QFile::WriteOnly|QFile::Text)){
        QTextStream out(&file);
        out << document.toString();
    }
    file.close();
}


void AeroISTWindow::on_actionLoad_Project_triggered()
{
    if ( measure_list->rowCount() != 0){
        message(tr("There is data in the project"));
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

void AeroISTWindow::ZeroSelectionChanged(const QModelIndex &current ,const QModelIndex &previous){
    Q_UNUSED(previous);
    if (current.isValid()){
        ui->actionDelete_Zero->setEnabled(true);
        ui->actionView_Zero_details->setEnabled(true);
    } else {
        ui->actionDelete_Zero->setEnabled(false);
        ui->actionView_Zero_details->setEnabled(false);
    }
}

void AeroISTWindow::on_listView_activated(const QModelIndex &index){
    ui->tableView->setModel( measure_list->at(index.row()));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab),measure_list->at(index.row())->name);
}

void AeroISTWindow::load_settings(void){
    settings = new QSettings("IST", "AeroIST");

    restoreState(settings->value("gui/state").toByteArray());
    restoreGeometry(settings->value("gui/geometry").toByteArray());

    ui->tabWidget->setCurrentIndex(settings->value("gui/tabwidget",0).toInt());
    ui->splitterLists->restoreState(settings->value("gui/splitterLists/state").toByteArray());
    ui->splitterGlobal->restoreState(settings->value("gui/splitterGlobal/state").toByteArray());
    ui->tableView->horizontalHeader()->setDefaultSectionSize( settings->value("gui/tablecolumnsize",80).toInt());
}

void AeroISTWindow::save_settings(void){
    settings->setValue("gui/state",saveState());
    settings->setValue("gui/geometry",saveGeometry());

    settings->setValue("gui/tabwidget",ui->tabWidget->currentIndex());
    settings->setValue("gui/splitterLists/state",ui->splitterLists->saveState());
    settings->setValue("gui/splitterGlobal/state",ui->splitterGlobal->saveState());

    settings->setValue("gui/tablecolumnsize",ui->tableView->horizontalHeader()->defaultSectionSize());
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



void AeroISTWindow::on_actionDelete_Zero_triggered()
{
     QModelIndex index = ui->listViewZero->currentIndex();
     if (zero_list->at(index) == ZeroThread && thread_status == ZERO_RUNNING){
         message(tr("Measuring is being done. Stop it to delete"));
         return;
     }

     if (measure_list->zeroUsed(zero_list->at(index)) == true){
         message(tr("This Zero is being used."));
         return;
     }

     // if the to be deleted model is in the table, unset
     if (zero_list->at(index) == ui->tableView->model()){
         ui->tableView->setModel(NULL);
         ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab),tr("Table"));
     }
     zero_list->deleteMeasure(index);

     if (index.isValid() && index.row() < zero_list->rowCount()){
         ui->listViewZero->setCurrentIndex(index);
     }
}


void AeroISTWindow::on_actionView_Zero_details_triggered()
{
    ZeroModel *zero;
    QModelIndex index = ui->listViewZero->currentIndex();
    if (index.isValid()){
        zero = zero_list->at(index);

        ZeroDetails *details = new ZeroDetails(zero,this);

        details->exec();
        delete details;
    }
}

void AeroISTWindow::on_listViewZero_activated(const QModelIndex &index){
    ui->tableView->setModel( zero_list->at(index.row()));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab),zero_list->at(index.row())->name);
}

void AeroISTWindow::on_actionNew_Zero_triggered(){
    if (m_thread && m_test){
        message(tr("Thread busy"));
        return;
    }
    if(thread_status == STOPPED){
        qDebug() << "action new zero" << thread_status;
        ZeroPreferences *zero_prefs;
        ZeroThread = new ZeroModel;


        zero_prefs = new ZeroPreferences(ZeroThread,settings,this);
        if (zero_prefs->exec() == QDialog::Rejected){
            delete zero_prefs;
            delete ZeroThread;
            return;
        }
        delete zero_prefs;
        zero_list->newMeasure(ZeroThread);
        QModelIndex index = zero_list->index(zero_list->rowCount()-1,0);
        ui->listViewZero->setCurrentIndex(index);
        ui->tableView->setModel(ZeroThread);

        m_test = new MeasureThread(ZeroThread);

        if (!m_thread)
            m_thread = new QThread(0);
        if (m_test->thread() != m_thread)
            m_test->moveToThread(m_thread);

        // start connects
        connect(m_thread, SIGNAL(started()), m_test, SLOT(produce()));

        // dump connect
        connect(m_test, SIGNAL(MeasureDone(measure)),ZeroThread, SLOT(GetMeasure(measure)));

        // stop connects
//        connect(ui->ZeroButton, SIGNAL(clicked()), m_test, SLOT(stop()));
        //    connect(m_thread,SIGNAL(finished()),this,SLOT(cleanup()));
        connect(m_thread,SIGNAL(finished()),this,SLOT(ZeroButton_cleanup()));

        thread_status = ZERO_RUNNING;
        qDebug() << "action new zero after start" << thread_status;

        m_thread->start();

//        QString text(tr("Stop "));
//        text.append( ZeroThread->name);
//        ui->ZeroButton->setText(text);
        return;
    }

}
/*
void AeroISTWindow::on_ZeroButton_clicked(){
    if (thread_status != STOPPED && thread_status != ZERO_RUNNING){
        message(tr("Thread busy"));
        return;
    }

    if (m_thread && m_test){
        return;
    }

    if(thread_status == STOPPED){
        QModelIndex index = ui->listViewZero->currentIndex();
        if (index.isValid() == false){
            message(tr("index not valid"));
            return;
        }
        ZeroThread = zero_list->at(index.row());         // get the index
        if (ZeroThread->rowCount(QModelIndex()) !=0){
            message(tr("Measure is not empty"));
            return;
        }
        m_test = new MeasureThread(ZeroThread);

        if (!m_thread)
            m_thread = new QThread(0);
        if (m_test->thread() != m_thread)
            m_test->moveToThread(m_thread);

        // start connects
        connect(m_thread, SIGNAL(started()), m_test, SLOT(produce()));

        // dump connect
        connect(m_test, SIGNAL(MeasureDone(measure)),ZeroThread, SLOT(GetMeasure(measure)));

        // stop connects
        connect(ui->ZeroButton, SIGNAL(clicked()), m_test, SLOT(stop()));
        //    connect(m_thread,SIGNAL(finished()),this,SLOT(cleanup()));
        connect(m_thread,SIGNAL(finished()),this,SLOT(ZeroButton_cleanup()));

        m_thread->start();
        thread_status = ZERO_RUNNING;
        QString text(tr("Stop "));
        text.append( ZeroThread->name);
        ui->ZeroButton->setText(text);
        return;
    }

}*/

void AeroISTWindow::ZeroButton_cleanup(){
    qDebug() << "zero button cleanup" << thread_status;
    if (thread_status == ZERO_RUNNING){
//        ui->ZeroButton->setText(tr("Start"));
        thread_status = STOPPED;
        cleanup();
        return;
    }
}

// helper fucntion to show a message
void AeroISTWindow::message(const QString &string){
    QMessageBox message;
    message.setText(string);
    message.exec();
}

// On exit warn if thread is runing and don't close
void AeroISTWindow::closeEvent(QCloseEvent *event){
    if (m_thread != 0 ) {
        message(tr("A thread is running. Stop it to quit"));
        event->ignore();
    } else {
        event->accept();
    }
}

// PLOT add, delete and clear
void AeroISTWindow::on_actionNew_Curve_triggered(){
    CurveNew *newcurve = new CurveNew(ui->qwtPlot, measure_list);
    if (newcurve->exec() == QDialog::Rejected){
        delete newcurve;
        return;
    }
}
void AeroISTWindow::on_actionDelete_Curve_triggered(){
    CurveDelete deletecurve(ui->qwtPlot);
    if(deletecurve.exec() == QDialog::Rejected){
        return;
    }
}
void AeroISTWindow::on_actionClear_Plot_triggered(){
    ui->qwtPlot->detachItems(QwtPlotItem::Rtti_PlotItem,true);
    ui->qwtPlot->replot();
}
// PLOT add, delete and clear - end

// View widgets
void AeroISTWindow::on_actionToolbar_toggled(bool arg1){
    ui->toolBar->setVisible(arg1);
}
void AeroISTWindow::on_actionMeasure_List_toggled(bool checked){
    ui->listView->setVisible(checked);
}
void AeroISTWindow::on_actionZero_List_toggled(bool arg1){
    ui->listViewZero->setVisible(arg1);
}
// View widgets - end

void AeroISTWindow::on_doubleSpinBoxWind_valueChanged(double arg1){
    emit set_wind(arg1);
}

void AeroISTWindow::on_doubleSpinBoxALpha_valueChanged(double arg1){
    emit set_alpha(arg1);
}

void AeroISTWindow::on_doubleSpinBoxBeta_valueChanged(double arg1){
    emit set_beta(arg1);
}

void AeroISTWindow::on_actionNames_in_Toolbar_toggled(bool arg1){
    if(arg1){
        ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    } else {
        ui->toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
}
