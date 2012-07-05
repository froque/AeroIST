#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <QVector>
#include <QAbstractTableModel>
#include <QTextStream>


#include "common.h"
#include "referencemodel.h"

#include "variable.h"


class MeasurementsModel  : public QAbstractTableModel
{
    Q_OBJECT

public:
    MeasurementsModel( QObject *parent=0);
    MeasurementsModel(QDomElement root,QObject *parent = 0);
    ~MeasurementsModel();
    void save_avg_csv(QTextStream *out,bool header);
    void save_csv(QTextStream *out,bool header);
    void save_raw_csv(QTextStream *out,bool header);
    void save_xml(QDomElement root);
    void load_xml(QDomElement root);

    QList<VariableModel*> variables;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    bool insertRow ( int row, const QModelIndex & parent = QModelIndex() );

    QVector<double>  vector_data(int index);

    // metadata about the measurement
    QString name;
    QString description;
    int measures_per_iteration;
    double settling_time;
    double end;
    double step;
    QString control;
    QString ref_name;
    int iterations;

public slots:
    void GetMeasure(QHash<QString,double> hash,QHash<QString,double> raw_hash);

private:
    void init(void);
};

#endif // MEASUREMENTS_H
