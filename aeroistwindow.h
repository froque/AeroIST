#ifndef AEROISTWINDOW_H
#define AEROISTWINDOW_H
#include <QtGui>
#include <QMainWindow>
#include <QFileSystemModel>
#include "measurementsmodel.h"
#include "zeromodel.h"
#include "measurethread.h"
#include "measurelist.h"
#include "zerolist.h"
#include "preferences.h"
#include "common.h"


namespace Ui {
    class AeroISTWindow;
}

class AeroISTWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AeroISTWindow(QWidget *parent = 0);
    ~AeroISTWindow();

signals:
    void set_alpha(double);
    void set_beta(double);
    void set_wind(double);
public slots:

private slots:
    // buttons
    void on_ThreadButton_clicked();
    void ThreadButton_cleanup();
    void cleanup();
//    void on_ZeroButton_clicked();
    void ZeroButton_cleanup();

    // export actions. to be improved!
    void on_actionPreferences_triggered();
    void on_actionExport_to_csv_triggered();
    void on_actionExportPlot_triggered();

    // measurement actions
    void on_actionNew_Measure_triggered();
    void on_actionDelete_Measure_triggered();
    void on_actionView_Measure_details_triggered();

    // project actions
    void on_actionSave_Project_triggered();
    void on_actionLoad_Project_triggered();
    void on_actionClear_Project_triggered();

    // View actions
    void on_actionToolbar_toggled(bool arg1);
    void on_actionMeasure_List_toggled(bool checked);
    void on_actionZero_List_toggled(bool arg1);

    // listviews and table
    void on_listView_activated(const QModelIndex &index);
    void on_listViewZero_activated(const QModelIndex &index);
    void selectionChanged(const QModelIndex &current,const QModelIndex &previous);
    void ZeroSelectionChanged(const QModelIndex &current,const QModelIndex &previous);

    // Zero actions
    void on_actionNew_Zero_triggered();
    void on_actionDelete_Zero_triggered();
    void on_actionView_Zero_details_triggered();

    // PLOT
    void on_actionNew_Curve_triggered();
    void on_actionClear_Plot_triggered();
    void on_actionDelete_Curve_triggered();

    // control spin boxes
    void on_doubleSpinBoxWind_valueChanged(double arg1);
    void on_doubleSpinBoxALpha_valueChanged(double arg1);
    void on_doubleSpinBoxBeta_valueChanged(double arg1);

    // enable/disable actions
    void on_actionNames_in_Toolbar_toggled(bool arg1);

private:
    void message(const QString &string);
    void start_loop( MeasurementsModel * measurement, MeasureThread *measureThread, const QObject *receiver, const char * slot);
    void stop_loop( MeasurementsModel * measurement, MeasureThread *measureThread, const QObject *receiver, const char * slot );
    void load_settings(void);
    void save_settings(void);

    QSettings *settings;
    Ui::AeroISTWindow *ui;
    Preferences * preferences;
    MeasureList *measure_list;
    ZeroList *zero_list;

    MeasurementsModel *measurementThread;
    ZeroModel* ZeroThread;
    QThread producerThread;
    MeasureThread *producer;
    MeasureThread *Zproducer;
    ThreadStatus thread_status;

    MeasureThread *m_test;
    QThread *m_thread;


protected:
    void closeEvent(QCloseEvent *event);

};

#endif // AEROISTWINDOW_H

