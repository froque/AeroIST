#ifndef MEASURELIST_H
#define MEASURELIST_H

#include <QtCore>
#include "measurementsmodel.h"
#include "common.h"
#include "zeromodel.h"

class MeasureList : public QAbstractListModel
{
    Q_OBJECT

public:
    MeasureList(QObject * parent = 0);

    int	rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    QVariant	data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;


    void newMeasure(MeasurementsModel * measure);
    void deleteMeasure(QModelIndex index);
    MeasurementsModel * at(int position);
    MeasurementsModel * at(QModelIndex index);

    bool zeroUsed(ZeroModel *zero);
//    Qt::DropActions supportedDropActions( void ) const ;
//    Qt::ItemFlags flags(const QModelIndex &index) const;
//    bool insertRow ( int row, const QModelIndex & parent = QModelIndex() );
//    bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
//    bool removeRow(int position, const QModelIndex &parent = QModelIndex());
//    bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex());
//    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
//    bool setItemData ( const QModelIndex & index, const QMap<int, QVariant> & roles );


    void save(QString fileName);
    void load(QString fileName);
    void clear(void);
private:
    QList<MeasurementsModel*> list;
//    QList<QAbstractTableModel*> list;
};

#endif // MEASURELIST_H
