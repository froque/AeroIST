#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <QVector>
#include <QAbstractTableModel>
#include <QTextStream>

#include "measure.h"
#include "common.h"
#include "zeromodel.h"

#define NVARS 12
#define NFORCES 6
class MeasurementsModel  : public QAbstractTableModel
{
    Q_OBJECT

public:
    MeasurementsModel( QObject *parent=0);
    MeasurementsModel(QDomElement root,QObject *parent = 0);

    void save(QTextStream *out);
    void save_csv(QTextStream *out,bool header);
    void save_xml(QDomElement root);
    void load_xml(QDomElement root);

    QVector<double> tempo;
    QVector<double> force[NFORCES];
    QVector<double> alpha;
    QVector<double> beta;
    QVector<double> motor;
    QVector<double> temp;
    QVector<double> wind;   // change NVARS if you add another

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
    int dvm_time;
    matrix_t matrix;
    int average_number;
    double settling_time;
    double start;
    double end;
    double step;
    double set_alpha;
    double set_beta;
    double set_motor;
    control_types_t control_type;
    double zero[NFORCES];
    QString zero_name;
    int n;

public slots:
    void GetMeasure(measure m);

};

#endif // MEASUREMENTS_H
