#ifndef ZEROMODEL_H
#define ZEROMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QTextStream>
#include <QDomElement>
#include <QStringList>

#include "measure.h"
#include "common.h"

#define NVARS_ZERO 6

class ZeroModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ZeroModel(QObject *parent = 0);
    explicit ZeroModel(QDomElement root,QObject *parent = 0);

    void load_xml(QDomElement root);
    void save_xml(QDomElement root);

    QVector<double> force[NVARS_ZERO];

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    bool insertRow ( int row, const QModelIndex & parent = QModelIndex() );

    // metadata about the measurement
    QString name;
    QString description;
    int dvm_time;
    matrix_t matrix;
    int average_number;
    double set_alpha;
    double set_beta;
    double set_motor;

public slots:
    void GetMeasure(QHash<QString, double>);
};

#endif // ZEROMODEL_H




