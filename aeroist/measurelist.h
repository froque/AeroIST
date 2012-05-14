#ifndef MEASURELIST_H
#define MEASURELIST_H

#include <QtCore>
#include "measurementsmodel.h"
#include "common.h"
#include "referencemodel.h"


class MeasureList : public QAbstractListModel
{
    Q_OBJECT

public:
    MeasureList(QObject * parent = 0);
    ~MeasureList();
    int	rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    QVariant	data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;


    void newMeasure(MeasurementsModel * measure);
    void deleteMeasure(QModelIndex index);
    MeasurementsModel * at(int position);
    MeasurementsModel * at(QModelIndex index);

    void save_xml(QDomElement root);
    void load_xml(QDomElement root);
    void clear(void);

private:
    QList<MeasurementsModel*> list;
};

#endif // MEASURELIST_H
