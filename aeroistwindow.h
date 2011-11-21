#ifndef AEROISTWINDOW_H
#define AEROISTWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include "measurementsmodel.h"
#include "measurethread.h"
#include <measurelist.h>
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

public slots:

private slots:
    // button actions (actually not actions)
    void on_ClearButton_clicked();
    void on_GraphButton_clicked();
    void on_ThreadButton_clicked();
    void on_ZeroButton_clicked();

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

    // listviews
    void on_listView_activated(const QModelIndex &index);
    void on_listViewZero_activated(const QModelIndex &index);
    void selectionChanged(const QModelIndex &current,const QModelIndex &previous);

    // Zero actions
    void on_actionNew_Zero_triggered();
    void on_actionDelete_Zero_triggered();
    void on_actionView_Zero_details_triggered();
    void on_actionZero_List_toggled(bool arg1);

    void cleanup();
private:
    void start_loop( MeasurementsModel * measurement, MeasureThread *measureThread, const QObject *receiver, const char * slot);
    void stop_loop( MeasurementsModel * measurement, MeasureThread *measureThread, const QObject *receiver, const char * slot );
    void load_settings(void);
    void save_settings(void);

    QSettings *settings;
    Ui::AeroISTWindow *ui;
    Preferences * preferences;
    MeasureList *measure_list;
    MeasureList *zero_list;

    MeasurementsModel *measurementThread;
    MeasurementsModel *ZeroThread;
    QThread producerThread;
    MeasureThread *producer;
    MeasureThread *Zproducer;
    ThreadStatus thread_status;

    MeasureThread *m_test;
    QThread *m_thread;
};

#endif // AEROISTWINDOW_H
