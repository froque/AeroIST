#include "curvenew.h"
#include "ui_curvenew.h"

#include <QMessageBox>
#include <qwt_plot_curve.h>
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
    ui->comboBox_x->setModel(list);
    ui->comboBox_y->setModel(list);

    PluginManager manager;
    QList<VariableMeta*> variables = manager.getListVariableMeta();
    foreach (VariableMeta *var, variables) {
        for(int k=0; k<var->get_num();k++){
            ui->listWidget_x->addItem(var->get_name(k));
            ui->listWidget_y->addItem(var->get_name(k));
        }
    }
    manager.destroyListVariableMeta(variables);

    ui->comboBoxStyle->addItem(tr("Solid Line"),Qt::SolidLine);
    ui->comboBoxStyle->addItem(tr("Dash Line"),Qt::DashLine);
    ui->comboBoxStyle->addItem(tr("Dot Line"),Qt::DotLine);
    ui->comboBoxStyle->addItem(tr("Dash Dot Line"),Qt::DashDotLine);
    ui->comboBoxStyle->addItem(tr("Dash Dot Dot Line"),Qt::DashDotDotLine);
    ui->comboBoxStyle->addItem(tr("custom Dash Line"),Qt::CustomDashLine);
    ui->comboBoxStyle->addItem(tr("No Pen"),Qt::NoPen);

    ui->comboBoxColor->addItems(QColor::colorNames());
    ui->comboBoxColor->setCurrentIndex(ui->comboBoxColor->findText("black"));

    QString st;
    st.append("Rair = 287.058\n");
    st.append("atm = 1.01325e-5\n");
    st.append("mmH2O = 9.80665\n");
    st.append("T0 = 273.15\n");
    st.append("g = 9.80665\n");
    st.append("rpm = 0.10471976");
    ui->label_consts->setText(st);

    QSettings settings;
    if(settings.contains(SETTINGS_GUI_CN_GEOMETRY)){
        restoreGeometry(settings.value(SETTINGS_GUI_CN_GEOMETRY).toByteArray());
    } else {
        adjustSize();
    }
}

CurveNew::~CurveNew(){
    QSettings settings;
    settings.setValue(SETTINGS_GUI_CN_GEOMETRY, saveGeometry());
    delete ui;
}

void CurveNew::on_listWidget_x_itemActivated(QListWidgetItem *item){
    ui->lineEdit_x->setText(ui->lineEdit_x->text() + item->text() + " ");
    ui->lineEdit_x->setFocus();
}

void CurveNew::on_listWidget_y_itemActivated(QListWidgetItem *item){
    ui->lineEdit_y->setText(ui->lineEdit_y->text() + item->text() + " ");
    ui->lineEdit_y->setFocus();
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

    QVector<double> x;
    QVector<double> y;

    try{
        p_x.DefineConst("Rair",287.058);
        p_x.DefineConst("atm",1.01325e-5);
        p_x.DefineConst("mmH2O",9.80665);
        p_x.DefineConst("T0", 273.15);
        p_x.DefineConst("g",9.80665);      // brederode
        //    p.DefineConst("Rair",287.05287); // brederode // there seems to be a lot of different values
        p_x.DefineConst("rpm", 0.10471976);

        p_y.DefineConst("Rair",287.058);
        p_y.DefineConst("atm",1.01325e-5);
        p_y.DefineConst("mmH2O",9.80665);
        p_y.DefineConst("T0", 273.15);
        p_y.DefineConst("g",9.80665);      // brederode
        p_y.DefineConst("rpm", 0.10471976);
        MeasurementsModel *model_x = mlist->at( ui->comboBox_x->currentIndex());
        MeasurementsModel *model_y = mlist->at( ui->comboBox_y->currentIndex());
        QVector<mu::value_type> row_x(model_x->columnCount());
        QVector<mu::value_type> row_y(model_y->columnCount());


        // get headers of columns
        for (int c=0; c< model_x->columnCount();c++){
            p_x.DefineVar(model_x->headerData(c,Qt::Horizontal,Qt::UserRole).toString().toStdString(),&row_x[c]);
            p_y.DefineVar(model_y->headerData(c,Qt::Horizontal,Qt::UserRole).toString().toStdString(),&row_y[c]);
        }

        p_x.SetExpr(ui->lineEdit_x->text().toStdString());
        p_y.SetExpr(ui->lineEdit_y->text().toStdString());

        // for each row, set and evaluate expression
        for(int r=0; r < model_x->rowCount(); r++){
            for(int c=0; c<model_x->columnCount();c++){
                row_x.replace(c, model_x->data(model_x->index(r,c),Qt::DisplayRole).toDouble());
            }
            x.append(p_x.Eval());
        }

        for(int r=0; r < model_y->rowCount(); r++){
            for(int c=0; c<model_y->columnCount();c++){
                row_y.replace(c, model_y->data(model_y->index(r,c),Qt::DisplayRole).toDouble());
            }
            y.append(p_y.Eval());
        }

    } catch (mu::Parser::exception_type &e) {
        QMessageBox msg;
        QString error = QString::fromStdString(e.GetMsg());
        msg.setText(tr("Problem with defined expressions.\n\n") + error);
        msg.exec();
        return;
    }

    QwtPlotCurve *curve = new QwtPlotCurve(ui->lineEdit->text());
    curve->setSamples(x,y);

    QPen pen;
    pen.setStyle((Qt::PenStyle)ui->comboBoxStyle->itemData(ui->comboBoxStyle->currentIndex()).toInt());
    pen.setColor(ui->comboBoxColor->currentText());
    curve->setPen(pen);

    curve->attach(plot);
    plot->replot();
    QDialog::accept();
}
