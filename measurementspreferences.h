#ifndef MEASUREMENTSPREFERENCES_H
#define MEASUREMENTSPREFERENCES_H

#include <QDialog>
#include <QSettings>
#include "measurementsmodel.h"
#include "zerolist.h"
#include <QSortFilterProxyModel>

namespace Ui {
    class MeasurementsPreferences;
}

class MeasurementsPreferences : public QDialog
{
    Q_OBJECT

public:
    MeasurementsPreferences(MeasurementsModel *measurement, ZeroList *list,QWidget *parent = 0);
    ~MeasurementsPreferences();

private slots:
    void accept(void);
    void maxminstep_enabled(int id);
    void on_combo_matrix_currentIndexChanged(int index);

private:
    Ui::MeasurementsPreferences *ui;
    MeasurementsModel *measurement;
    ZeroList *list;
    QSortFilterProxyModel *proxyfilter;
};

#endif // MEASUREMENTSPREFERENCES_H
