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
        qDebug() << "index is valid";
        plot->itemList().at(ui->listWidget->currentIndex().row())->detach();
        plot->replot();
        QDialog::accept();
    }
}
