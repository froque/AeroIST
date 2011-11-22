#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <QTime>
#include <QVector>
#include <QAbstractTableModel>
#include <QTextStream>
#include "measure.h"
#include "common.h"

#define NVARS 11

class MeasurementsModel  : public QAbstractTableModel
{
    Q_OBJECT

public:
    MeasurementsModel(QObject *parent=0);

    void load(QTextStream *in);
    void save(QTextStream *out);
    void save_csv(QTextStream *out,bool header);

//    QVector<QTime> tempo;
//    QVector<double> raw_tempo;
    QVector<double> tempo;
    QVector<double> force[6];
    QVector<double> alpha;
    QVector<double> beta;
    QVector<double> wind;
    QVector<double> temp;
//    QVector<double> raw_force[6];
//    QVector<double> raw_alpha;
//    QVector<double> raw_beta;
//    QVector<double> raw_wind;
//    QVector<double> raw_temp;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;


    // metadata about the measurement
    QString name;
    int dvm_time;
    matrix_t matrix;
    int average_number;
    double settling_time;
    double min;
    double max;
    double step;
    control_types_t control_type;
    bool isZero;
    MeasurementsModel *zero;
    int n;

signals:
    void produceMeasure();

public slots:
    void GetMeasure(measure m);

};

#endif // MEASUREMENTS_H
