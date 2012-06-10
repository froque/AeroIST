#include "curvenew.h"
#include "ui_curvenew.h"

#include <QMessageBox>
#include <qwt_plot_curve.h>
#include <QPushButton>

CurveNew::CurveNew(QwtPlot *plot, MeasureList *list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewCurve),
    list(list),
    plot(plot)
{
    ui->setupUi(this);
    ui->comboBox_x->setModel(list);
    ui->comboBox_y->setModel(list);

    ui->comboBoxStyle->addItem(tr("Solid Line"),Qt::SolidLine);
    ui->comboBoxStyle->addItem(tr("Dash Line"),Qt::DashLine);
    ui->comboBoxStyle->addItem(tr("Dot Line"),Qt::DotLine);
    ui->comboBoxStyle->addItem(tr("Dash Dot Line"),Qt::DashDotLine);
    ui->comboBoxStyle->addItem(tr("Dash Dot Dot Line"),Qt::DashDotDotLine);
    ui->comboBoxStyle->addItem(tr("custom Dash Line"),Qt::CustomDashLine);
    ui->comboBoxStyle->addItem(tr("No Pen"),Qt::NoPen);

    ui->comboBoxColor->addItems(QColor::colorNames());
    ui->comboBoxColor->setCurrentIndex(ui->comboBoxColor->findText("black"));

}

CurveNew::~CurveNew(){
    delete ui;
}

void CurveNew::on_comboBox_x_currentIndexChanged(int index){
    ui->listWidget_x->clear();
    model = list->at(index);
    for (int k=0;k< model->columnCount();k++){
        ui->listWidget_x->addItem(model->headerData(k,Qt::Horizontal,Qt::UserRole+1).toString());
    }

}

void CurveNew::on_comboBox_y_currentIndexChanged(int index){
    ui->listWidget_y->clear();
    model = list->at(index);
    for (int k=0;k< model->columnCount();k++){
        ui->listWidget_y->addItem(model->headerData(k,Qt::Horizontal,Qt::UserRole+1).toString());
    }

}
void CurveNew::accept(){
    if (ui->listWidget_x->currentIndex().isValid() && ui->listWidget_y->currentIndex().isValid() && ui->lineEdit->text().isEmpty()!=true){
        x = model->vector_data(ui->listWidget_x->currentIndex().row());
        y = model->vector_data(ui->listWidget_y->currentIndex().row());

        QwtPlotCurve *curve = new QwtPlotCurve(ui->lineEdit->text());
        curve->setSamples(x,y);
        QPen pen;
        pen.setStyle((Qt::PenStyle)ui->comboBoxStyle->itemData(ui->comboBoxStyle->currentIndex()).toInt());
        pen.setColor(ui->comboBoxColor->currentText());
        curve->setPen(pen);

//        curve->setYAxis(QwtPlot::yRight);
        curve->attach(plot);
        plot->replot();
        QDialog::accept();
    } else {
        QMessageBox msg;
        msg.setText(tr("Fill in a title and choose X and Y"));
        msg.exec();
    }
}
