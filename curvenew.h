#ifndef NEWCURVE_H
#define NEWCURVE_H

#include <QDialog>
#include "measurelist.h"
#include "qwt/qwt_plot.h"

namespace Ui {
    class NewCurve;
}

class CurveNew : public QDialog
{
    Q_OBJECT

public:
    explicit CurveNew(QwtPlot *plot, MeasureList *list, QWidget *parent = 0);
    ~CurveNew();

//    QwtPlotCurve* get_curve(void);

public slots:
    void accept(void);

private slots:

    void on_comboBox_x_currentIndexChanged(int index);
    void on_comboBox_y_currentIndexChanged(int index);

private:
    Ui::NewCurve *ui;
    MeasureList *list;
    QVector<double> x;
    QVector<double> y;
    MeasurementsModel* model;
    QwtPlot *plot;
};

#endif // NEWCURVE_H
