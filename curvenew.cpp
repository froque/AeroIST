#include "curvenew.h"
#include "ui_newcurve.h"

#include <QMessageBox>
#include <qwt/qwt_plot_curve.h>

CurveNew::CurveNew(QwtPlot *plot, MeasureList *list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewCurve),
    list(list),
    plot(plot)
{
    ui->setupUi(this);
    ui->comboBox_x->setModel(list);
    ui->comboBox_y->setModel(list);
}

CurveNew::~CurveNew()
{
    delete ui;
}

//QwtPlotCurve* CurveNew::get_curve(void){
//    QwtPlotCurve *curve = new QwtPlotCurve(ui->lineEdit->text());
//    curve->setSamples(x,y);
//    return curve;
//}

void CurveNew::on_comboBox_x_currentIndexChanged(int index)
{
    ui->listWidget_x->clear();
    model = list->at(index);
    for (int k=0;k< model->columnCount();k++){
        ui->listWidget_x->addItem(model->headerData(k,Qt::Horizontal,Qt::DisplayRole).toString());
    }

}

void CurveNew::on_comboBox_y_currentIndexChanged(int index)
{
    ui->listWidget_y->clear();
    model = list->at(index);
    for (int k=0;k< model->columnCount();k++){
        ui->listWidget_y->addItem(model->headerData(k,Qt::Horizontal,Qt::DisplayRole).toString());
    }

}
void CurveNew::accept(){
    if (ui->listWidget_x->currentIndex().isValid() && ui->listWidget_y->currentIndex().isValid() && ui->lineEdit->text().isEmpty()!=true){
        x = model->vector_data(ui->listWidget_x->currentIndex().row());
        y = model->vector_data(ui->listWidget_y->currentIndex().row());
        QwtPlotCurve *curve = new QwtPlotCurve(ui->lineEdit->text());
        curve->setSamples(x,y);
        curve->attach(plot);
        plot->replot();
        QDialog::accept();
    } else {
        QMessageBox msg;
        msg.setText(tr("Fill in a title and choose X and Y"));
        msg.exec();
    }
}
