#include "curvenew.h"
#include "ui_curvenew.h"

#include <QMessageBox>
#include <qwt_plot_curve.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_interval_symbol.h>
#include <QPushButton>
#include <muParser/muParser.h>
#include <pluginmanager.h>

CurveNew::CurveNew(QwtPlot *plot, MeasureList *list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewCurve),
    mlist(list),
    plot(plot)
{
    ui->setupUi(this);
    ui->comboBoxModel->setModel(list);

    QStringList strlist;
    PluginManager manager;
    QList<VariableMeta*> variables = manager.getListVariableMeta();
    foreach (VariableMeta *var, variables) {
        for(int k=0; k<var->get_num();k++){
            strlist.append(var->get_name(k));
            ui->listWidgetVars->addItem(var->get_name(k));
        }
    }
//    ui->label_vars->setText(strlist.join("\n"));
    manager.destroyListVariableMeta(variables);

    ui->comboBoxStyle->addItem(tr("Solid Line"),Qt::SolidLine);
    ui->comboBoxStyle->addItem(tr("Dash Line"),Qt::DashLine);
    ui->comboBoxStyle->addItem(tr("Dot Line"),Qt::DotLine);
    ui->comboBoxStyle->addItem(tr("Dash Dot Line"),Qt::DashDotLine);
    ui->comboBoxStyle->addItem(tr("Dash Dot Dot Line"),Qt::DashDotDotLine);

    ui->comboBoxColor->addItems(QColor::colorNames());
    ui->comboBoxColor->setCurrentIndex(ui->comboBoxColor->findText("black"));

    QStringList strlist_consts;
    strlist_consts.append(QString::fromUtf8("Rair = 287.058 J kg\u207b\u00b9 K\u207b\u00b9"));
    strlist_consts.append("atm = 101325 Pa");
    strlist_consts.append("mmH2O = 9.80665 Pa");
    strlist_consts.append("T0 = 273.15 K");
    strlist_consts.append(QString::fromUtf8("g = 9.80665 m/s\u00B2"));
    strlist_consts.append("rpm = 0.10471976 Hz");
//    ui->label_consts->setText(strlist_consts.join("\n"));
    ui->listWidgetConsts->addItems(strlist_consts);

    QSettings settings;
    // load geometry
    if(settings.contains(SETTINGS_GUI_CN_GEOMETRY)){
        restoreGeometry(settings.value(SETTINGS_GUI_CN_GEOMETRY).toByteArray());
    } else {
        adjustSize();
    }
    // load previous values for expressions and model
    ui->checkBoxAvg->setChecked( settings.value(SETTINGS_GUI_CN_CHECKBOX_AVG, true).toBool());
    ui->checkBoxError->setChecked( settings.value(SETTINGS_GUI_CN_CHECKBOX_ERR, true).toBool());
    ui->checkBoxError->setEnabled(ui->checkBoxAvg->isChecked());
    ui->lineEdit_x->setText(settings.value(SETTINGS_GUI_CN_LINEEDIT_X).toString());
    ui->lineEdit_y->setText(settings.value(SETTINGS_GUI_CN_LINEEDIT_Y).toString());
    int index = ui->comboBoxModel->findText(settings.value(SETTINGS_GUI_CN_COMBOMODEL).toString());
    if (index != -1){
        ui->comboBoxModel->setCurrentIndex(index);
    }
}

CurveNew::~CurveNew(){
    QSettings settings;
    settings.setValue(SETTINGS_GUI_CN_COMBOMODEL,ui->comboBoxModel->currentText());
    settings.setValue(SETTINGS_GUI_CN_LINEEDIT_X,ui->lineEdit_x->text());
    settings.setValue(SETTINGS_GUI_CN_LINEEDIT_Y,ui->lineEdit_y->text());
    settings.setValue(SETTINGS_GUI_CN_CHECKBOX_AVG,ui->checkBoxAvg->isChecked());
    settings.setValue(SETTINGS_GUI_CN_CHECKBOX_ERR,ui->checkBoxError->isChecked());
    settings.setValue(SETTINGS_GUI_CN_GEOMETRY, saveGeometry());
    delete ui;
}

void CurveNew::accept(void){
    mu::Parser p_x,p_y;

    if(mlist->rowCount() <= 0){
        QMessageBox msg;
        msg.setText(tr("Empty measurements list"));
        msg.exec();
        return;
    }

    if (ui->lineEdit_x->text().isEmpty() || ui->lineEdit_y->text().isEmpty() || ui->lineEdit->text().isEmpty()){
        QMessageBox msg;
        msg.setText(tr("Fill in a title and choose X and Y"));
        msg.exec();
        return;
    }

    // results of expression evaluations
    QVector<double> v_x;
    QVector<double> v_y;
    MeasurementsModel *model = mlist->at( ui->comboBoxModel->currentIndex());

    QMap<QString,double> consts;
    consts["Rair"] = 287.058;
    consts["atm"] = 101325;
    consts["mmH2O"] = 9.80665;
    consts["T0"] = 273.15;
    consts["g"] = 9.80665;      // brederode
    consts["rpm"] = 0.10471976;

    try{
        // define muparser constants
        QMapIterator<QString, double> i(consts);
        while (i.hasNext()) {
            i.next();
            p_x.DefineConst(i.key().toStdString(),i.value());
            p_y.DefineConst(i.key().toStdString(),i.value());
        }

        // get headers of columns to define muparser variables
        QVector<mu::value_type> row(model->columnCount());
        for (int c=0; c< model->columnCount();c++){
            p_x.DefineVar(model->headerData(c,Qt::Horizontal,Qt::UserRole).toString().toStdString(),&row[c]);
            p_y.DefineVar(model->headerData(c,Qt::Horizontal,Qt::UserRole).toString().toStdString(),&row[c]);
        }

        // define muparser expressions
        p_x.SetExpr(ui->lineEdit_x->text().toStdString());
        p_y.SetExpr(ui->lineEdit_y->text().toStdString());

        // for each row, set and evaluate muparser expressions
        for(int r=0; r < model->rowCount(); r++){
            for(int c=0; c<model->columnCount();c++){
                row.replace(c, model->data(model->index(r,c),Qt::DisplayRole).toDouble());
            }
            v_x.append(p_x.Eval());
            v_y.append(p_y.Eval());
        }
    } catch (mu::Parser::exception_type &e) {
        QMessageBox msg;
        QString error = QString::fromStdString(e.GetMsg());
        msg.setText(tr("Problem with defined expressions.\n\n") + error);
        msg.exec();
        return;
    } // done with muparser

    // determine what type of curve to graph
    if(ui->checkBoxAvg->isChecked()){
        double avg_x,min_x,max_x;
        double avg_y,min_y,max_y;
        int R,N;
        QVector<QwtIntervalSample> v_samples;

        QVector<double> v_xavg;
        QVector<double> v_yavg;
        QVector<double> v_xmin;
        QVector<double> v_xmax;
        QVector<double> v_ymin;
        QVector<double> v_ymax;
        R = v_x.size();
        N = model->measures_per_iteration;
        // cycle through rows and average for N measures
        for (int r=0;r < (R - R%N); r+=N){
            avg_x = avg_y = 0;
            max_x = min_x = v_x.at(r);
            max_y = min_y = v_y.at(r);
            for (int i=r ; i < r+N;i++){
                avg_x += v_x.at(i);
                avg_y += v_y.at(i);
                max_x = qMax(v_x.at(i),max_x);
                max_y = qMax(v_y.at(i),max_y);
                min_x = qMin(v_x.at(i),min_x);
                min_y = qMin(v_y.at(i),min_y);
            }
            avg_x = avg_x / N;
            avg_y = avg_y / N;
            v_xavg.append(avg_x);
            v_yavg.append(avg_y);
            v_xmax.append(max_x);
            v_ymax.append(max_y);
            v_xmin.append(min_x);
            v_ymin.append(min_y);
            v_samples.append(QwtIntervalSample(avg_x,min_y,max_y));
        }

        if(ui->checkBoxError->isChecked()){
            QwtPlotIntervalCurve *intCurve = new QwtPlotIntervalCurve(ui->lineEdit->text());
            intCurve->setStyle(QwtPlotIntervalCurve::NoCurve);

            QwtIntervalSymbol *symbol = new QwtIntervalSymbol( QwtIntervalSymbol::Bar );
            QPen pen;
            pen.setStyle((Qt::PenStyle)ui->comboBoxStyle->itemData(ui->comboBoxStyle->currentIndex()).toInt());
            pen.setColor(ui->comboBoxColor->currentText());
            symbol->setPen(pen);

            intCurve->setSymbol( symbol );
            intCurve->setSamples(v_samples);
            intCurve->attach(plot);

        } else {
            QwtPlotCurve *curve = new QwtPlotCurve(ui->lineEdit->text());
            curve->setSamples(v_xavg,v_yavg);
            QPen pen;
            pen.setStyle((Qt::PenStyle)ui->comboBoxStyle->itemData(ui->comboBoxStyle->currentIndex()).toInt());
            pen.setColor(ui->comboBoxColor->currentText());
            curve->setPen(pen);
            curve->attach(plot);
        }
    } else{
        QwtPlotCurve *curve = new QwtPlotCurve(ui->lineEdit->text());
        curve->setSamples(v_x,v_y);
        QPen pen;
        pen.setStyle((Qt::PenStyle)ui->comboBoxStyle->itemData(ui->comboBoxStyle->currentIndex()).toInt());
        pen.setColor(ui->comboBoxColor->currentText());
        curve->setPen(pen);
        curve->attach(plot);
    }
    plot->replot();
    QDialog::accept();
}

void CurveNew::on_checkBoxAvg_toggled(bool checked)
{
    ui->checkBoxError->setEnabled(checked);
}
