#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "common.h"
#include "force.h"
#include "angle.h"
#include "temperature.h"
#include "motor.h"
#include "wind.h"

#include <QTime>
#include <QElapsedTimer>
#include "measure.h"
#include "measurementsmodel.h"
#include "variable.h"

class MeasureThread : public QObject
{
    Q_OBJECT
public:
    explicit MeasureThread(MeasurementsModel *measurement, QObject *parent = 0);
    explicit MeasureThread(ZeroModel *measurement, QObject *parent = 0);
    ~MeasureThread();
    void isReady(void);

signals:
  void MeasureDone(QHash<QString,double>);

public slots:
    void produce(void);
    void stop();
    void control_alpha(double);
    void control_beta(double);
    void control_motor(double);

private:
    void set_m(void);
    void set_initial(void);
    void read_m(void);
//    void set_m_virtual(void);
//    void read_m_virtual(void);
//    void clear_m(void);
    double GetRandomMeasurement(void);
    QElapsedTimer timer;
    measure m;
    QHash<QString,double> m_hash;

    int average_number;
    double settling_time;
    double start;
    double end;
    double step;
    double current;
    double set_alpha;
    double set_beta;
    double set_motor;
    control_types_t control_type;
    bool isZero;
    QString name;
    int n;
    measure zero;

    bool m_stop;
    QThread* m_parent_thread;
    bool virtual_measures;

    // Variables
//    Force *force;
//    Alpha *alpha;
//    Beta *beta;
//    Temperature *temperature;
//    Motor *motor;
//    Wind *wind;

    Variable *force;
    Variable *alpha;
    Variable *beta;
    Variable *temperature;
    Variable *motor;
    Variable *wind;

    QList<Variable*> variables;
};

#endif // MYTHREAD_H
