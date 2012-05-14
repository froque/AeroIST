#include "curvedelete.h"
#include "ui_deleteplot.h"

CurveDelete::CurveDelete(QwtPlot *plot,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeletePlot),
    plot(plot)
{
    ui->setupUi(this);
    foreach(QwtPlotItem *item,plot->itemList()){
        ui->listWidget->addItem(item->title().text());
    }
}

CurveDelete::~CurveDelete()
{
    delete ui;
}

void CurveDelete::accept(){
    if(ui->listWidget->currentIndex().isValid()){
        QwtPlotItem *curve = plot->itemList().at(ui->listWidget->currentIndex().row());
        curve->detach();
        delete curve;
        plot->replot();
        QDialog::accept();
    }
}
