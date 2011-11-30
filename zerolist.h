#ifndef ZEROLIST_H
#define ZEROLIST_H

#include <QAbstractListModel>
#include "common.h"
#include "zeromodel.h"

class ZeroList : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ZeroList(QObject *parent = 0);

    int	rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    QVariant	data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;


    void newMeasure(ZeroModel * measure);
    void deleteMeasure(QModelIndex index);
    ZeroModel * at(int position);
    ZeroModel * at(QModelIndex index);

//    void save(QString fileName);
//    void load(QString fileName);
//    void clear(void);

signals:

public slots:

private:
    QList<ZeroModel*> list;
};

#endif // ZEROLIST_H
