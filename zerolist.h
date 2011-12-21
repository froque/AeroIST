#ifndef ZEROLIST_H
#define ZEROLIST_H

#include <QAbstractListModel>
#include "common.h"
#include "zeromodel.h"
#include "QDomElement"

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

    void save_xml(QDomElement root);
    void load_xml(QDomElement root);
    void clear(void);

private:
    QList<ZeroModel*> list;
};

#endif // ZEROLIST_H
