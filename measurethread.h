#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "common.h"
#include "force.h"
#include "angle.h"
#include "temperature.h"
#include "wind.h"
#if DEBUG
#include <QThread>
#endif // DEBUG

#include <QTime>
#include <QElapsedTimer>
#include "measure.h"
#include "measurementsmodel.h"


#ifdef DEBUG
class Helper: public QThread {
public:
        static void msleep(int ms)
        {
                QThread::msleep(ms);
        }
        static void sleep(int s)
        {
                QThread::sleep(s);
        }
};
#endif //DEBUG

class MeasureThread : public QObject//: public QThread
{
    Q_OBJECT
public:
    explicit MeasureThread(MeasurementsModel *measurement, QObject *parent = 0);
    explicit MeasureThread(ZeroModel *measurement, QObject *parent = 0);
    ~MeasureThread();
signals:
    void MeasureDone(measure m);
    void message(QString);

public slots:
    void produce(void);
//    void start_timer(void);
    void stop();
    void control_alpha(double);
    void control_beta(double);
    void control_wind(double);
private:
    void set_m(void);
    void set_initial(void);
    void read_m(void);
    void set_m_virtual(void);
    void read_m_virtual(void);
    void clear_m(void);
    double GetRandomMeasurement(void);
    QElapsedTimer timer;
    measure m;
    int average_number;
    double settling_time;
    double start;
    double end;
    double step;
    double current;
    double set_alpha;
    double set_beta;
    double set_wind;
    control_types_t control_type;
    bool isZero;
    QString name;
    int n;
    int k;
    measure zero;

    bool m_stop;
    QThread* m_parent_thread;

    // Variables
    Force *force;
    Alpha *alpha;
    Beta *beta;
    Temperature *temperature;
    Wind *wind;
    bool virtual_measures;
};

#endif // MYTHREAD_H
