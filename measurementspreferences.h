#ifndef MEASUREMENTSPREFERENCES_H
#define MEASUREMENTSPREFERENCES_H

#include <QDialog>
#include <QSettings>
#include "measurementsmodel.h"
#include "measurelist.h"

namespace Ui {
    class MeasurementsPreferences;
}

class MeasurementsPreferences : public QDialog
{
    Q_OBJECT

public:
    MeasurementsPreferences(MeasurementsModel *measurement, MeasureList *list, QSettings *settings,QWidget *parent = 0);
    ~MeasurementsPreferences();



private slots:
    void on_buttonBox_accepted();
    void test_input();
    void maxminstep_enabled(int id);

private:
    Ui::MeasurementsPreferences *ui;
    MeasurementsModel *measurement;
    MeasureList *list;
};

#endif // MEASUREMENTSPREFERENCES_H
