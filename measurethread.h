#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "common.h"
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

signals:
    void MeasureDone(measure m);

public slots:
    void produce(void);
//    void start_timer(void);
    void stop();

private:
    void read_m(void);
    void clear_m(void);
    double GetRandomMeasurement(void);
    QElapsedTimer timer;
    measure m;
    int average_number;
    double settling_time;
    double min;
    double max;
    double step;
    double current;
    control_types_t control_type;
    bool isZero;
    QString name;
    int n;
    int k;

    bool m_stop;
    QThread* m_parent_thread;

};

#endif // MYTHREAD_H
