#ifndef MEASUREMENTDETAILS_H
#define MEASUREMENTDETAILS_H

#include <QDialog>
#include "measurementsmodel.h"
#include "common.h"

#ifdef DEBUG
#include <QDebug>
#endif //DEBUG

namespace Ui {
    class MeasurementDetails;
}

class MeasurementDetails : public QDialog
{
    Q_OBJECT

public:
    explicit MeasurementDetails(MeasurementsModel *measurement, QWidget *parent = 0);
    ~MeasurementDetails();

private:
    Ui::MeasurementDetails *ui;
};

#endif // MEASUREMENTDETAILS_H
