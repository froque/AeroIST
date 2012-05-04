#include "aeroistwindow.h"
#include "ui_aeroistwindow.h"

#include <QMetaType>
#include <QFile>
#include <QFileDialog>
#include <qwt_legend.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_renderer.h>
#include <QMessageBox>
#include <QtXml>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

#include "measurementspreferences.h"
#include "referencepreferences.h"
#include "measurementdetails.h"
#include "referencedetails.h"
#include "curvenew.h"
#include "curvedelete.h"

#include <QDebug>

#include <stdexcept>

AeroISTWindow::AeroISTWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AeroISTWindow)
{
    ui->setupUi(this);

    // Set some standard (freedesktop only) Icons
    ui->actionNew_Measure->setIcon(QIcon(":/resources/ruler_add.svg"));
    ui->actionDelete_Measure->setIcon(QIcon(":/resources/ruler_remove.svg"));
    ui->actionView_Measure_details->setIcon(QIcon(":/resources/ruler_view.svg"));
    ui->actionLoad_Project->setIcon(QIcon(":/resources/document-open.svg"));
    ui->actionSave_Project->setIcon(QIcon(":/resources/document-save-as.svg"));
    ui->actionDelete_Reference->setIcon(QIcon(":/resources/ref_remove.svg"));
    ui->actionNew_Reference->setIcon(QIcon(":/resources/ref_add.svg"));
    ui->actionView_Reference_details->setIcon(QIcon(":/resources/ref_view.svg"));

    // Set the models
    measure_list = new MeasureList();
    ui->listView->setModel(measure_list);
    reference_list = new ReferenceList();
    ui->listViewReference->setModel(reference_list);
    proxy = new ProxyModel;
    ui->tableView->setModel(proxy);

    // listview personalization. It couldn't be done from the .ui file
    QItemSelectionModel *selection = ui->listView->selectionModel();
    connect(selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(selectionChanged(QModelIndex,QModelIndex)));
    ui->listView->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->listView->insertAction(0,ui->actionView_Measure_details);
    ui->listView->insertAction(0,ui->actionDelete_Measure);
    ui->listView->insertAction(0,ui->actionExport_to_csv);
    ui->listView->insertAction(0,ui->actionExport_raw_to_csv);

    selection = ui->listViewReference->selectionModel();
    connect(selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(ReferenceSelectionChanged(QModelIndex,QModelIndex)));
    ui->listViewReference->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->listViewReference->insertAction(0,ui->actionView_Reference_details);
    ui->listViewReference->insertAction(0,ui->actionDelete_Reference);

    // load settings from .ini file
    load_settings();

    qRegisterMetaType<QHash<QString,double> >("QHash<QString,double>");
    thread_status = STOPPED;

    m_thread = 0;
    m_test = 0;

    // Plot legend
    QwtLegend *legend = new QwtLegend;
    legend->setItemMode(QwtLegend::CheckableItem);
    ui->qwtPlot->insertLegend(legend);
//    ui->qwtPlot->enableAxis(QwtPlot::yRight);
    connect(ui->qwtPlot,SIGNAL(legendChecked(QwtPlotItem*,bool)),this,SLOT(plot_legend(QwtPlotItem*,bool)));

    // set spinboxes range and step
    QList<VariableMeta*> variables;
    Factory *factory;
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        factory = qobject_cast<Factory*>( loader.instance());
        if(factory){
            VariableMeta *meta = factory->CreateVariableMeta();
            if (meta != NULL){
                variables.append(meta);
            }
        }
    }

    QDoubleSpinBox *spin;
    QLabel *label;
    int column = ui->horizontalLayout_2->indexOf(ui->ManualButton);
    foreach (VariableMeta *var, variables) {
        if(var->is_controlable()){
            for (int k=0; k< var->get_num(); k++){
                label = new QLabel(var->get_name_tr(k).append(" (").append(var->get_units(k).append(")")));
                ui->horizontalLayout_2->insertWidget( column,label);
                column++;
                spin = new QDoubleSpinBox;
                spin->setRange(var->get_lower_bound(k), var->get_upper_bound(k));
                spin->setSingleStep(var->get_default_step(k));
                spin->setEnabled(false);
                spin->setObjectName(var->get_name(k));
                list_spins.append(spin);
                ui->horizontalLayout_2->insertWidget( column,spin);
                column ++;
            }
        }
    }
    ui->ManualButton->setEnabled(false);
}

AeroISTWindow::~AeroISTWindow()
{
    save_settings();
    delete ui;
    delete measure_list;
    delete reference_list;
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

    if (ui->actionTable_follows_Start->isChecked()){
        proxy->setSourceModel(measurementThread);
    }

    try{
        m_test = new MeasureThread(measurementThread);
    }
    catch ( const std::runtime_error & err ) {
        message(tr("Error opening devices: ") + err.what());
        return ;
    }

    try{
        m_test->isReady();
    }
    catch ( const std::runtime_error & err ) {
        message(tr("Error using devices: ") + err.what());
        return ;
    }


    if (!m_thread)
        m_thread = new QThread(0);
    if (m_test->thread() != m_thread)
        m_test->moveToThread(m_thread);

    // start connects
    connect(m_thread, SIGNAL(started()), m_test, SLOT(produce()));

    // dump connect
    connect(m_test,SIGNAL(MeasureDone(QHash<QString,double>,QHash<QString,double>)), measurementThread, SLOT(GetMeasure(QHash<QString,double>,QHash<QString,double>)));


    // stop connects
    connect(ui->ThreadButton, SIGNAL(clicked()), m_test, SLOT(stop()));
    connect(m_thread,SIGNAL(finished()),this,SLOT(ThreadButton_cleanup()));

    // pass the values from comboboxes to the thread. only for free control
    connect(this,SIGNAL(set_variable(QHash<QString,double>)),m_test,SLOT(manual_control(QHash<QString,double>)));


    if (measurementThread->control == ""){
        ui->ManualButton->setEnabled(true);
        foreach (QDoubleSpinBox *spin, list_spins) {
            spin->setEnabled(true);
            foreach (VariableModel * var, measurementThread->variables) {
                for(int k=0; k<var->meta->get_num(); k++){
                    if(var->meta->get_name(k) == spin->objectName()){
                        spin->setValue(var->start.at(k));
                    }
                }
            }
        }
    }
    if ( (measurementThread->control == "" && measurementThread->iterations >0) || measurementThread->control != ""){
        if(ui->progressBar->isVisible()){
            ui->progressBar->setEnabled(true);
            connect(m_test, SIGNAL(progress(int)),ui->progressBar,SLOT(setValue(int)));
        }
    }

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
        ui->ManualButton->setEnabled(false);
        ui->progressBar->setEnabled(false);
        foreach (QDoubleSpinBox *spin, list_spins) {
            spin->setEnabled(false);
        }
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
    QSettings settings;
    fileName = QFileDialog::getSaveFileName(this, tr("Export mesurements"), settings.value(SETTINGS_PROJECT_FOLDER).toString(),"*.csv");

    if(fileName.endsWith(".csv",Qt::CaseInsensitive) == false){
        fileName.append(".csv");
    }

    QFile data(fileName);
    if (data.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&data);       
        measurement->save_csv(&out,true);
    }
    data.close();
}

void AeroISTWindow::on_actionExport_raw_to_csv_triggered()
{
    int row =  ui->listView->currentIndex().row();
    if (row < 0 || row > measure_list->rowCount()){
        return;
    }
    MeasurementsModel *measurement;
    measurement = measure_list->at(row);

    QString fileName;
    QSettings settings;
    fileName = QFileDialog::getSaveFileName(this, tr("Export raw mesurements"), settings.value(SETTINGS_PROJECT_FOLDER).toString(),"*.csv");

    if(fileName.endsWith(".csv",Qt::CaseInsensitive) == false){
        fileName.append(".csv");
    }

    QFile data(fileName);
    if (data.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&data);
        measurement->save_raw_csv(&out,true);
    }
    data.close();
}


// Save current plot to file based on extension
void AeroISTWindow::on_actionExportPlot_triggered(){
    QString fileName;
    QSettings settings;
    fileName = QFileDialog::getSaveFileName(this, tr("Export plot"), settings.value(SETTINGS_PROJECT_FOLDER).toString(), "" );

    QwtPlotRenderer* renderer = new QwtPlotRenderer();
    renderer->renderDocument(ui->qwtPlot,fileName,QSize(297,210),300);  //A4 size, 300 DPI
}

void AeroISTWindow::on_actionNew_Measure_triggered(){
    MeasurementsModel *measurement;
    measurement = new MeasurementsModel;

    MeasurementsPreferences *meas_prefs = new MeasurementsPreferences( measurement, reference_list , this);
    if (meas_prefs->exec() == QDialog::Rejected ){
        delete measurement;
        delete meas_prefs;
        return ;
    }
    measure_list->newMeasure(measurement);
    ui->listView->setCurrentIndex(measure_list->index(measure_list->rowCount()-1,0));
    delete meas_prefs;
}

void AeroISTWindow::on_actionDelete_Measure_triggered(){
    QModelIndex index = ui->listView->currentIndex();

    if (measure_list->at(index) == measurementThread && thread_status == MEASURE_RUNNING){
        message(tr("Measuring is being done. Stop it to delete."));
        return;
    }

    // if the to be deleted model is shown in the table, unset
    if (measure_list->at(index) == proxy->sourceModel()){
        proxy->setSourceModel(NULL);
        ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab),tr("Table"));
    }
    measure_list->deleteMeasure(index);
    if (index.isValid() && index.row() < measure_list->rowCount()){
        ui->listView->setCurrentIndex(index);
    }
}

void AeroISTWindow::on_actionSave_Project_as_triggered(){
    if (thread_status != STOPPED){
        message(tr("Measuring is being done. Stop it to save to disk."));
        return;
    }
    QSettings settings;
    project_filename = QFileDialog::getSaveFileName(this, tr("Save Project"), settings.value(SETTINGS_PROJECT_FOLDER).toString(),"*.xml");
    save_xml(project_filename);
}

void AeroISTWindow::on_actionSave_Project_triggered(){
    if (thread_status != STOPPED){
        message(tr("Measuring is being done. Stop it to save to disk"));
        return;
    }
    if (project_filename.isNull() || project_filename.isEmpty()){
        QSettings settings;
        project_filename = QFileDialog::getSaveFileName(this, tr("Save Project"), settings.value(SETTINGS_PROJECT_FOLDER).toString(), "*.xml");
    }
    save_xml(project_filename);
}

void AeroISTWindow::save_xml(QString fileName){
    QDomDocument document;
    QDomElement root = document.createElement(TAG_PROJECT);
    document.appendChild(root);

    reference_list->save_xml(root);
    measure_list->save_xml(root);

    if(fileName.endsWith(".xml",Qt::CaseInsensitive) == false){
        fileName.append(".xml");
    }

    QFile file(fileName);
    if (file.open(QFile::WriteOnly|QFile::Text)){
        QTextStream out(&file);
        out << document.toString();
    }
    file.close();
}

void AeroISTWindow::on_actionLoad_Project_triggered(){
    if ( measure_list->rowCount() != 0){
        message(tr("There is data in the project"));
        return;
    }
    QString fileName;
    QSettings settings;
    fileName = QFileDialog::getOpenFileName(this, tr("Load Project"), settings.value(SETTINGS_PROJECT_FOLDER).toString(), "*.xml");
    if (fileName.isNull() || fileName.isEmpty()){
        return;
    }
    load_xml(fileName);
}

void AeroISTWindow::load_xml(QString fileName){

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        message(tr("Could not open file"));
        return;
    }

    QXmlSchema schema;
    QSettings settings;

    schema.load( QUrl::fromLocalFile(qApp->applicationDirPath() + "/" +  settings.value(SETTINGS_SCHEMA_FILE,SETTINGS_SCHEMA_FILE_DEFAULT).toString()) );
    if (schema.isValid()){
        QXmlSchemaValidator validator( schema );
        if(!validator.validate(QUrl::fromLocalFile(fileName)) && settings.value(SETTINGS_SCHEMA_CONFIRM,false).toBool() == true){
            message(tr("Couldn't confirm xml file. Continuing without guaranty."));
//            return;
        }
    } else {
        message(tr("Couldn't confirm schema file"));
        return;
    }

    project_filename = fileName;

    QDomDocument document;
    document.setContent(&file);
    file.close();
    QDomElement root = document.firstChildElement();

    reference_list->load_xml(root);
    measure_list->load_xml(root);
}

void AeroISTWindow::on_actionClear_Project_triggered(){
    measure_list->clear();
    reference_list->clear();
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab),tr("Table"));
}

// Open preferences dialog
void AeroISTWindow::on_actionPreferences_triggered(){
    Preferences preferences(this);
    preferences.exec();
}

void AeroISTWindow::selectionChanged(const QModelIndex &current ,const QModelIndex &previous){
    Q_UNUSED(previous);
    if (current.isValid()){
        ui->ThreadButton->setEnabled(true);
        ui->actionDelete_Measure->setEnabled(true);
        ui->actionView_Measure_details->setEnabled(true);
        ui->actionExport_to_csv->setEnabled(true);
        ui->actionExport_raw_to_csv->setEnabled(true);
    } else {
        ui->ThreadButton->setEnabled(false);
        ui->actionDelete_Measure->setEnabled(false);        // BUG unity, fixme later
        ui->actionView_Measure_details->setEnabled(false);
        ui->actionExport_to_csv->setEnabled(false);
        ui->actionExport_raw_to_csv->setEnabled(false);
    }
}

void AeroISTWindow::ReferenceSelectionChanged(const QModelIndex &current ,const QModelIndex &previous){
    Q_UNUSED(previous);
    if (current.isValid()){
        ui->actionDelete_Reference->setEnabled(true);
        ui->actionView_Reference_details->setEnabled(true);
    } else {
        ui->actionDelete_Reference->setEnabled(false);
        ui->actionView_Reference_details->setEnabled(false);
    }
}

void AeroISTWindow::on_listView_activated(const QModelIndex &index){
    proxy->setSourceModel( measure_list->at(index.row()));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab),measure_list->at(index.row())->name);
}

void AeroISTWindow::load_settings(void){
    QSettings settings;

    // necessary settings
    settings.setValue(SETTINGS_PROJECT_FOLDER,settings.value(SETTINGS_PROJECT_FOLDER,QDir::homePath()).toString());
    settings.setValue(SETTINGS_DEFAULT_MEASURES_ITERATION,settings.value(SETTINGS_DEFAULT_MEASURES_ITERATION,1));
    settings.setValue(SETTINGS_DEFAULT_SETTLING_TIME,settings.value(SETTINGS_DEFAULT_SETTLING_TIME,0));

    // optional settings
    restoreState(settings.value(SETTINGS_GUI_STATE).toByteArray());
    restoreGeometry(settings.value(SETTINGS_GUI_GEOMETRY).toByteArray());

    ui->tabWidget->setCurrentIndex(settings.value(SETTINGS_GUI_TABWIDGET,0).toInt());
    ui->splitterLists->restoreState(settings.value(SETTINGS_GUI_SPLITTERLISTS_STATE).toByteArray());
    ui->splitterGlobal->restoreState(settings.value(SETTINGS_GUI_SPLITTERGLOBAL_STATE).toByteArray());
    ui->tableView->horizontalHeader()->setDefaultSectionSize( settings.value(SETTINGS_GUI_TABLECOLUMNSIZE,80).toInt());

    QAction * action;
    QList<QAction*> actions;

    actions.append(ui->menuEdit->actions());
    actions.append(ui->menuExport->actions());
    actions.append(ui->menuPlot->actions());
    actions.append(ui->menuProject->actions());
    actions.append(ui->menuView->actions());

    foreach (action, actions) {
        if (!action->isSeparator() ){
            action->setShortcut(QKeySequence(settings.value(SETTINGS_GUI_SHORTCUT + action->objectName() ,action->shortcut().toString()).toString()));
        }
    }

    foreach (action, ui->menuView->actions()) {
        if (!action->isSeparator() && action->isCheckable() ){
            action->setChecked(settings.value(SETTINGS_GUI_ACTIONCHECKABLE + action->objectName(),action->isChecked()).toBool());
        }
    }
}

void AeroISTWindow::save_settings(void){
    QSettings settings;
    settings.setValue(SETTINGS_GUI_STATE,saveState());
    settings.setValue(SETTINGS_GUI_GEOMETRY,saveGeometry());

    settings.setValue(SETTINGS_GUI_TABWIDGET,ui->tabWidget->currentIndex());
    settings.setValue(SETTINGS_GUI_SPLITTERLISTS_STATE,ui->splitterLists->saveState());
    settings.setValue(SETTINGS_GUI_SPLITTERGLOBAL_STATE,ui->splitterGlobal->saveState());
    settings.setValue(SETTINGS_GUI_TABLECOLUMNSIZE,ui->tableView->horizontalHeader()->defaultSectionSize());

    QAction * action;
    QList<QAction*> actions;

    actions.append(ui->menuEdit->actions());
    actions.append(ui->menuExport->actions());
    actions.append(ui->menuPlot->actions());
    actions.append(ui->menuProject->actions());
    actions.append(ui->menuView->actions());

    foreach (action, actions) {
        if (!action->isSeparator() ){
        settings.setValue(SETTINGS_GUI_SHORTCUT + action->objectName() ,action->shortcut().toString());
        }
    }
    foreach (action, ui->menuView->actions()) {
        if (!action->isSeparator() ){
            settings.setValue(SETTINGS_GUI_ACTIONCHECKABLE + action->objectName() ,action->isChecked());
        }
    }
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

void AeroISTWindow::on_actionDelete_Reference_triggered()
{
     QModelIndex index = ui->listViewReference->currentIndex();
     if (reference_list->at(index) == referenceThread && thread_status == REFERENCE_RUNNING){
         message(tr("Measuring is being done. Stop it to delete"));
         return;
     }

     // if the to be deleted model is in the table, unset
     if (reference_list->at(index) == proxy->sourceModel()){
         proxy->setSourceModel(NULL);
         ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab),tr("Table"));
     }
     reference_list->deleteMeasure(index);

     if (index.isValid() && index.row() < reference_list->rowCount()){
         ui->listViewReference->setCurrentIndex(index);
     }
}


void AeroISTWindow::on_actionView_Reference_details_triggered(){
    ReferenceModel *ref;
    QModelIndex index = ui->listViewReference->currentIndex();
    if (index.isValid()){
        ref = reference_list->at(index);

        ReferenceDetails *details = new ReferenceDetails(ref,this);

        details->exec();
        delete details;
    }
}

void AeroISTWindow::on_listViewReference_activated(const QModelIndex &index){
    proxy->setSourceModel(reference_list->at(index.row()));
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->tab),reference_list->at(index.row())->name);
}

void AeroISTWindow::on_actionNew_Reference_triggered(){
    if (m_thread && m_test){
        message(tr("Thread busy"));
        return;
    }
    if(thread_status == STOPPED){
        ReferencePreferences *ref_prefs;
        referenceThread = new ReferenceModel;

        ref_prefs = new ReferencePreferences(referenceThread, this);
        if (ref_prefs->exec() == QDialog::Rejected){
            delete ref_prefs;
            delete referenceThread;
            return;
        }
        delete ref_prefs;
        reference_list->newMeasure(referenceThread);
        QModelIndex index = reference_list->index(reference_list->rowCount()-1,0);
        ui->listViewReference->setCurrentIndex(index);

        if (ui->actionTable_follows_Start->isChecked()){
            proxy->setSourceModel(referenceThread);
        }

        try {
            m_test = new MeasureThread(referenceThread);
        }
        catch ( const std::runtime_error & err ) {
            message(tr("Error opening devices: ") + err.what());
            reference_list->deleteMeasure(index);
            return ;
        }

        try{
            m_test->isReady();
        }
        catch ( const std::runtime_error & err ) {
            message(tr("Error using devices: ") + err.what());
            reference_list->deleteMeasure(index);
            return ;
        }


        if (!m_thread)
            m_thread = new QThread(0);
        if (m_test->thread() != m_thread)
            m_test->moveToThread(m_thread);

        // start connects
        connect(m_thread, SIGNAL(started()), m_test, SLOT(produce()));

        // dump connect
        connect(m_test, SIGNAL(MeasureDone(QHash<QString,double>,QHash<QString,double>)),referenceThread, SLOT(GetMeasure(QHash<QString,double>,QHash<QString,double>)));

        // stop connects
        connect(m_thread,SIGNAL(finished()),this,SLOT(ReferenceButton_cleanup()));

        thread_status = REFERENCE_RUNNING;

        m_thread->start();
        return;
    }
}

void AeroISTWindow::ReferenceButton_cleanup(){
    qDebug() << "zero button cleanup" << thread_status;
    if (thread_status == REFERENCE_RUNNING){
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
        QMessageBox mess;
        mess.setText(tr("A measurement is running"));
        mess.setInformativeText(tr("Do you really want to quit? Hardware may not be properly closed."));
        mess.setIcon(QMessageBox::Warning);
        mess.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        mess.setDefaultButton(QMessageBox::Cancel);
        switch (mess.exec()){
        case QMessageBox::Cancel:
            event->ignore();
            break;
        case QMessageBox::Ok:
            event->accept();
        }
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

void AeroISTWindow::plot_legend(QwtPlotItem* plotItem,bool on){
    plotItem->setVisible(!on);
    ui->qwtPlot->replot();
}

void AeroISTWindow::on_actionClear_Plot_triggered(){
    ui->qwtPlot->detachItems(QwtPlotItem::Rtti_PlotItem,true);
    ui->qwtPlot->replot();
}

// View widgets
void AeroISTWindow::on_actionToolbar_toggled(bool arg1){
    ui->toolBar->setVisible(arg1);
}

void AeroISTWindow::on_actionNames_in_Toolbar_toggled(bool arg1){
    if(arg1){
        ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    } else {
        ui->toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
}

void AeroISTWindow::on_actionLine_numbers_in_table_toggled(bool arg1){
    ui->tableView->verticalHeader()->setVisible(arg1);
}

void AeroISTWindow::on_actionRaw_data_toggled(bool arg1){
    proxy->show_raw = arg1;

    // refresh the table
    ui->tableView->setModel(NULL);
    ui->tableView->setModel(proxy);
}

void AeroISTWindow::on_ManualButton_clicked(){
    QHash<QString,double> hash;
    foreach (QDoubleSpinBox *spin, list_spins) {
        hash[spin->objectName()] = spin->value();
    }
    emit set_variable(hash);
}

void AeroISTWindow::on_actionShow_Lists_toggled(bool arg1){
    ui->splitterLists->setVisible(arg1);
}
