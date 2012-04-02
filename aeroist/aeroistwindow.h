#ifndef AEROISTWINDOW_H
#define AEROISTWINDOW_H

#include <QtGui>
#include <QMainWindow>
#include "measurementsmodel.h"
#include "referencemodel.h"
#include "measurethread.h"
#include "measurelist.h"
#include "referencelist.h"
#include "preferences.h"
#include "common.h"
#include "qwt_plot_item.h"
#include "proxymodel.h"

namespace Ui {
    class AeroISTWindow;
}

class AeroISTWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AeroISTWindow(QWidget *parent = 0);
    ~AeroISTWindow();

    void load_xml(QString fileName);

signals:
    void set_variable(QHash<QString,double>);

public slots:

private slots:
    // buttons
    void on_ThreadButton_clicked();
    void ThreadButton_cleanup();
    void cleanup();
    void ReferenceButton_cleanup();

    // Edit menu
    void on_actionPreferences_triggered();

    // export actions. to be improved!
    void on_actionExport_to_csv_triggered();
    void on_actionExport_raw_to_csv_triggered();
    void on_actionExportPlot_triggered();

    // measurement actions
    void on_actionNew_Measure_triggered();
    void on_actionDelete_Measure_triggered();
    void on_actionView_Measure_details_triggered();

    // project actions
    void on_actionSave_Project_triggered();
    void on_actionSave_Project_as_triggered();
    void on_actionLoad_Project_triggered();
    void on_actionClear_Project_triggered();

    // View actions
    void on_actionToolbar_toggled(bool arg1);
    void on_actionShow_Lists_toggled(bool arg1);
    void on_actionNames_in_Toolbar_toggled(bool arg1);
    void on_actionLine_numbers_in_table_toggled(bool arg1);
    void on_actionRaw_data_toggled(bool arg1);

    // listviews and table
    void on_listView_activated(const QModelIndex &index);
    void on_listViewReference_activated(const QModelIndex &index);
    void selectionChanged(const QModelIndex &current,const QModelIndex &previous);
    void ReferenceSelectionChanged(const QModelIndex &current,const QModelIndex &previous);

    // References actions
    void on_actionNew_Reference_triggered();
    void on_actionDelete_Reference_triggered();
    void on_actionView_Reference_details_triggered();

    // PLOT
    void on_actionNew_Curve_triggered();
    void on_actionClear_Plot_triggered();
    void on_actionDelete_Curve_triggered();
    void plot_legend(QwtPlotItem* plotItem,bool on);

    // control spin boxes
    void on_ManualButton_clicked();



private:
    void message(const QString &string);
    void load_settings(void);
    void save_settings(void);
    void save_xml(QString fileName);

    Ui::AeroISTWindow *ui;
    MeasureList *measure_list;
    ReferenceList *reference_list;

    ProxyModel *proxy;
    MeasurementsModel *measurementThread;
    ReferenceModel* referenceThread;
    ThreadStatus thread_status;

    MeasureThread *m_test;
    QThread *m_thread;
    QString project_filename;

    QList<QDoubleSpinBox*> list_spins;

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // AEROISTWINDOW_H

