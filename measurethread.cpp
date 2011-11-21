#include "measurethread.h"
#include <cstdlib> // for RAND_MAX


#include <QDebug>

MeasureThread::MeasureThread(MeasurementsModel *measurement,QObject *parent) :
    QObject(parent),
    average_number(measurement->average_number),
    settling_time(measurement->settling_time),
    min(measurement->min),
    max(measurement->max),
    step(measurement->step),
    current(measurement->min),
    control_type(measurement->control_type),
    isZero(measurement->isZero)
{
    qDebug() << this << isZero << measurement->name;
    name = measurement->name;
}

void MeasureThread::start_timer(void){
    qsrand(QTime::currentTime().second()*QTime::currentTime().msec());
    timer.start();
}

void MeasureThread::produce(){
qDebug() << name;
    if (current > max && isZero == false){
//        emit thread_finished();
        this->thread()->quit();
        return;
    }
    m.alpha = 0;
    m.beta = 0;
    m.force[0] = 0;
    m.force[1] = 0;
    m.force[2] = 0;
    m.force[3] = 0;
    m.force[4] = 0;
    m.force[5] = 0;
    m.temp=0;
    m.wind=0;

    m.tempo = timer.elapsed()/1000.0;
//    qDebug() << m.tempo;
    switch (control_type){
    case NONE :
        for (int k=0; k< average_number ; k++ ) m.alpha += GetRandomMeasurement();
        for (int k=0; k< average_number ; k++ ) m.beta += GetRandomMeasurement();
        for (int k=0; k< average_number ; k++ ) m.wind += GetRandomMeasurement();
        for (int k =0; k<6; k++){
            for (int n=0; n< average_number ; n++ )
                m.force[k] += GetRandomMeasurement();
        }
        for (int k=0; k< average_number ; k++ ) m.temp += GetRandomMeasurement();
        break;
    case ALPHA:
        Helper::msleep(settling_time*1000);
        for (int k=0; k< average_number ; k++ ) m.alpha += current ;//* 1.01 * GetRandomMeasurement();;
        for (int k=0; k< average_number ; k++ ) m.beta += GetRandomMeasurement();
        for (int k=0; k< average_number ; k++ ) m.wind += GetRandomMeasurement();
        for (int k =0; k<6; k++){
            for (int n=0; n< average_number ; n++ )
                m.force[k] += (k+1) * m.alpha + 0.01 * GetRandomMeasurement();
        }
        for (int k=0; k< average_number ; k++ ) m.temp += - 2 * m.alpha  + 0.01 *GetRandomMeasurement();
        break;
    case BETA:
        Helper::msleep(settling_time*1000);
        for (int k=0; k< average_number ; k++ ) m.alpha += GetRandomMeasurement();
        for (int k=0; k< average_number ; k++ ) m.beta += current ;//* 1.01 * GetRandomMeasurement();;
        for (int k=0; k< average_number ; k++ ) m.wind += GetRandomMeasurement();
        for (int k =0; k<6; k++){
            for (int n=0; n< average_number ; n++ )
                m.force[k] += (k+1) * m.beta + 0.01 * GetRandomMeasurement();
        }
        for (int k=0; k< average_number ; k++ ) m.temp += - 2 * m.beta  + 0.01 *GetRandomMeasurement();
        break;
    case WIND:
        Helper::msleep(settling_time*1000);
        for (int k=0; k< average_number ; k++ ) m.alpha += GetRandomMeasurement();
        for (int k=0; k< average_number ; k++ ) m.beta += GetRandomMeasurement();
        for (int k=0; k< average_number ; k++ ) m.wind += current ;//* 1.01 * GetRandomMeasurement();;
        for (int k =0; k<6; k++){
            for (int n=0; n< average_number ; n++ )
                m.force[k] += (k+1) * m.wind + 0.01 * GetRandomMeasurement();
        }
        for (int k=0; k< average_number ; k++ ) m.temp += - 2 * m.wind  + 0.01 *GetRandomMeasurement();
        break;
    }


    // Divide by N
    m.alpha = m.alpha / average_number;
    m.beta = m.beta  / average_number;
    for (int k=0; k<6; k++){
        m.force[k] = m.force[k] / average_number;
    }
    m.temp = m.temp / average_number;
    m.wind = m.wind / average_number;

    Helper::msleep( 0.5 * average_number *  1000.0 * qrand() / RAND_MAX);

    emit MeasureDone(m);
    if (control_type != NONE){
        current = current + step;
    }
    if (isZero == true){
        qDebug() << "zero. quiting";
        this->thread()->quit();
        return;
    }
}

double MeasureThread::GetRandomMeasurement(void)
{
    return 10.0 * qrand() / RAND_MAX;
}
