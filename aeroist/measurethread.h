#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "common.h"

#include <QTime>
#include <QElapsedTimer>

#include "measurementsmodel.h"
#include "variable.h"

class MeasureThread : public QObject
{
    Q_OBJECT
public:
    explicit MeasureThread(MeasurementsModel *measurement, QObject *parent = 0);
    explicit MeasureThread(ReferenceModel *measurement, QObject *parent = 0);
    ~MeasureThread();
    void isReady(void);

signals:
  void MeasureDone(QHash<QString,double>,QHash<QString,double>);
  void progress(int);
  void message(QString);
  void newData(bool);

public slots:
    void produce(void);
    void stop();
    void manual_control(QHash<QString,double>);

private:
    void set_m(void);
    void set_initial(void);
    void read_m(void);

    QHash<QString,double> m_hash,raw_hash;

    int measures_per_iteration;
    double settling_time;
    double end;
    double step;
    double start;
    double current;

    QString control;

    QString name;
    int iterations;

    // manual variables
    bool man_control;
    QHash<QString, double> man_hash;

    bool m_stop;
    QThread* m_parent_thread;

    // Variables
    QList<VariableHardware*> variables;
};

#endif // MYTHREAD_H
