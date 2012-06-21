#include "curvedelete.h"
#include "ui_deleteplot.h"
#include <QSettings>
#include "common.h"

CurveDelete::CurveDelete(QwtPlot *plot,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeletePlot),
    plot(plot)
{
    ui->setupUi(this);
    foreach(QwtPlotItem *item,plot->itemList()){
        ui->listWidget->addItem(item->title().text());
    }
    QSettings settings;
    if(settings.contains(SETTINGS_GUI_CD_GEOMETRY)){
        restoreGeometry(settings.value(SETTINGS_GUI_CD_GEOMETRY).toByteArray());
    } else {
        adjustSize();
    }
}

CurveDelete::~CurveDelete(){
    QSettings settings;
    settings.setValue(SETTINGS_GUI_CD_GEOMETRY, saveGeometry());
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
