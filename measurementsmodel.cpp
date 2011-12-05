#include <QtGlobal>
#include <QStringList>
#include "measurementsmodel.h"


#ifdef DEBUG
#include "QDebug"
#endif // DEBUG

MeasurementsModel::MeasurementsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    name="";
    dvm_time=0;
    matrix=MIDDLE;
    average_number=0;
    settling_time=0;
    min=0;
    max=0;
    step=0;
    control_type = NONE;
    zero  = 0;
    n = 0;
}

void MeasurementsModel::load(QTextStream *in)
{
    int size;
    name = in->readLine();
    dvm_time = in->readLine().toInt();
    matrix = (matrix_t) in->readLine().toInt();
    size = in->readLine().toInt();
    beginInsertRows(QModelIndex(),0,size);
    QStringList var;
    for (int row =0; row < size ;row++){
        var = in->readLine().split(";");

        tempo.append(var.at(0).toDouble());
        force[0].append(var.at(1).toDouble());
        force[1].append(var.at(2).toDouble());
        force[2].append(var.at(3).toDouble());
        force[3].append(var.at(4).toDouble());
        force[4].append(var.at(5).toDouble());
        force[5].append(var.at(6).toDouble());
        alpha.append(var.at(7).toDouble());
        beta.append(var.at(8).toDouble());
        wind.append(var.at(9).toDouble());
        temp.append(var.at(10).toDouble());
    }
    endInsertRows();
}

void MeasurementsModel::save(QTextStream *out )
{
    *out << name << endl;
    *out << dvm_time << endl;
    *out << matrix << endl;
    int size = rowCount(QModelIndex());
    *out << size << endl;

    save_csv(out,false);

}

void MeasurementsModel::save_csv(QTextStream *out,bool header){
    int rows = rowCount(QModelIndex());
    int columns = columnCount(QModelIndex());

    if (header == true){
        for (int column=0; column < columns; column++){
            *out << headerData(column,Qt::Horizontal,Qt::DisplayRole).toString();
            if (column != columns-1){
                *out << ";";
            }
        }
        *out << endl;
    }
    QModelIndex ind;
    for (int row =0; row < rows;row++){
        for (int column=0; column < columns; column++){
            ind = index(row,column,QModelIndex());
            *out << data(ind,Qt::DisplayRole).toDouble();
            if (column != columns-1){
                *out << ";";
            }
        }
        *out << endl;
    }
}

void MeasurementsModel::GetMeasure(measure m)
{
    beginInsertRows(QModelIndex(), tempo.size(), tempo.size()+1);
    for (int k=0;k<6;k++ ){
        force[k].append(m.force[k]);
    }
    alpha.append( m.alpha);
    beta.append( m.beta );
    wind.append( m.wind );
    temp.append( m.temp );
    tempo.append(m.tempo);

    endInsertRows();

}

int MeasurementsModel::columnCount(const QModelIndex &parent = QModelIndex()) const
{
    Q_UNUSED(parent);
    return NVARS;
}

int MeasurementsModel::rowCount(const QModelIndex &parent = QModelIndex()) const
{
    Q_UNUSED(parent);
    return force[0].size();
}

QVariant MeasurementsModel::data(const QModelIndex &index, int role = Qt::DisplayRole) const
{
    if (!index.isValid()){
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        int row = index.row();
        switch (index.column()) {
            case 0:
                return tempo.value(row);
            case 1:
                return force[0].value(row);
            case 2:
                return force[1].value(row);
            case 3:
                return force[2].value(row);
            case 4:
                return force[3].value(row);
            case 5:
                return force[4].value(row);
            case 6:
                return force[5].value(row);
            case 7:
                return alpha.value(row);
            case 8:
                return beta.value(row);
            case 9:
                return wind.value(row);
            case 10:
                return temp.value(row);
        }
    }
    return QVariant();
}

QVariant MeasurementsModel::headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
               return tr("Time");
            case 1:
                return tr("Fx");
            case 2:
                return tr("Fy");
            case 3:
                return tr("Fz");
            case 4:
                return tr("Mx");
            case 5:
                return tr("My");
            case 6:
                return tr("Mz");
            case 7:
                return tr("Alpha");
            case 8:
                return tr("Beta");
            case 9:
                return tr("Wind");
            case 10:
                return tr("Temperature");
            default:
                return QVariant();
        }
    }
    return QVariant();
}
