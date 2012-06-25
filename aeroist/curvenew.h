#ifndef NEWCURVE_H
#define NEWCURVE_H

#include <QDialog>
#include "measurelist.h"
#include <qwt_plot.h>
#include <QListWidget>

namespace Ui {
    class NewCurve;
}

class CurveNew : public QDialog
{
    Q_OBJECT

public:
    explicit CurveNew(QwtPlot *plot, MeasureList *list, QWidget *parent = 0);
    ~CurveNew();

public slots:
    void accept(void);

private slots:
    void on_listWidget_x_itemActivated(QListWidgetItem *item);
    void on_listWidget_y_itemActivated(QListWidgetItem *item);

private:
    Ui::NewCurve *ui;
    MeasureList *mlist;
    QwtPlot *plot;
};

#endif // NEWCURVE_H
