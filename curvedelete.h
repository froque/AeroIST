#ifndef DELETEPLOT_H
#define DELETEPLOT_H

#include <QDialog>
#include <qwt/qwt_plot.h>

namespace Ui {
    class DeletePlot;
}

class CurveDelete : public QDialog
{
    Q_OBJECT

public:
    explicit CurveDelete(QwtPlot *plot, QWidget *parent = 0);
    ~CurveDelete();
    void accept();
private:
    Ui::DeletePlot *ui;
    QwtPlot *plot;
};

#endif // DELETEPLOT_H
