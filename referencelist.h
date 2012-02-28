#ifndef REFERENCELIST_H
#define REFERENCELIST_H

#include <QAbstractListModel>
#include "common.h"
#include "referencemodel.h"
#include "QDomElement"

class ReferenceList : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ReferenceList(QObject *parent = 0);

    int	rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    QVariant	data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    void newMeasure(ReferenceModel * measure);
    void deleteMeasure(QModelIndex index);
    ReferenceModel * at(int position);
    ReferenceModel * at(QModelIndex index);

    void save_xml(QDomElement root);
    void load_xml(QDomElement root);
    void clear(void);

private:
    QList<ReferenceModel*> list;
};

#endif // REFERENCELIST_H
