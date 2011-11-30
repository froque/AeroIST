#include "zeromodel.h"

ZeroModel::ZeroModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    name="";
    dvm_time=0;
    matrix=MIDDLE;
    average_number=0;
    n = 0;
}

int ZeroModel::rowCount(const QModelIndex &parent) const{
    Q_UNUSED(parent);
    return force[0].size();
}

int ZeroModel::columnCount(const QModelIndex &parent) const{
    Q_UNUSED(parent);
    return NVARS_ZERO;
}

QVariant ZeroModel::data(const QModelIndex &index, int role) const{
    if (!index.isValid()){
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        int row = index.row();
        switch (index.column()) {
            case 0:
                return force[0].value(row);
            case 1:
                return force[1].value(row);
            case 2:
                return force[2].value(row);
            case 3:
                return force[3].value(row);
            case 4:
                return force[4].value(row);
            case 5:
                return force[5].value(row);
        }
    }
    return QVariant();
}

QVariant ZeroModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Fx");
            case 1:
                return tr("Fy");
            case 2:
                return tr("Fz");
            case 3:
                return tr("Mx");
            case 4:
                return tr("My");
            case 5:
                return tr("Mz");
            default:
                return QVariant();
        }
    }
    return QVariant();
}

void ZeroModel::GetMeasure(measure m){
    beginInsertRows(QModelIndex(), 0, 0);
    for (int k=0;k<6;k++ ){
        force[k].append(m.force[k]);
    }
    endInsertRows();
}
