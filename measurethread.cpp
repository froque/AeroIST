#include "measurethread.h"

#include <cstdlib> // for RAND_MAX

#include <QEventLoop>
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
    n(measurement->n)
{
    m_parent_thread = thread();
    isZero = false;
    if (isZero == false){
        if (measurement->zero == 0){
            qWarning("zero is not set to the measurement");
            return;
        }
        zero.force[0] = measurement->zero->force[0].first();
        zero.force[1] = measurement->zero->force[1].first();
        zero.force[2] = measurement->zero->force[2].first();
        zero.force[3] = measurement->zero->force[3].first();
        zero.force[4] = measurement->zero->force[4].first();
        zero.force[5] = measurement->zero->force[5].first();
    }

    force = new Force(measurement->matrix,measurement->dvm_time);
    alpha = new Alpha;
    beta = new Beta;
}

MeasureThread::MeasureThread(ZeroModel *measurement,QObject *parent) :
    QObject(parent),
    average_number(measurement->average_number)
{
    m_parent_thread = thread();
    isZero = true;
    n=1;
    force = new Force(measurement->matrix,measurement->dvm_time);
    alpha = new Alpha;
    beta = new Beta;
}

MeasureThread::~MeasureThread(){
    delete force;
}

void MeasureThread::produce(){
    m_stop = false;
    timer.start();
    QEventLoop eloop;
    k = 1;
    while(!m_stop) {
        read_m();
        if (isZero == false){
            subtract(&m,zero);
        }
        if (control_type != NONE){
            current = current + step;
        } else {
            if (n != 0 && k>= n ){
                m_stop = true;
            }
        }
        if (current > max ){
            m_stop = true;
        }
        k++;
        emit MeasureDone(m);

        eloop.processEvents(QEventLoop::AllEvents, 50);
    }
    if (m_parent_thread != thread())
    {
        thread()->quit();
        moveToThread(m_parent_thread);
    }
}

double MeasureThread::GetRandomMeasurement(void){
    return 10.0 * qrand() / RAND_MAX;
}



void MeasureThread::read_m(void){

    clear_m();

    m.tempo = timer.elapsed()/1000.0;
    switch (control_type){
    case NONE :
        for (int k=0; k< average_number ; k++ ) m.wind += GetRandomMeasurement();
        for (int k=0; k< average_number ; k++ ) m.temp += GetRandomMeasurement();
        break;
    case ALPHA:
        Helper::msleep(settling_time*1000);
        for (int k=0; k< average_number ; k++ ) m.wind += GetRandomMeasurement();
        for (int k=0; k< average_number ; k++ ) m.temp += - 2 * m.alpha  + 0.01 *GetRandomMeasurement();
        break;
    case BETA:
        Helper::msleep(settling_time*1000);

        for (int k=0; k< average_number ; k++ ) m.wind += GetRandomMeasurement();
        for (int k=0; k< average_number ; k++ ) m.temp += - 2 * m.beta  + 0.01 *GetRandomMeasurement();
        break;
    case WIND:
        Helper::msleep(settling_time*1000);
        for (int k=0; k< average_number ; k++ ) m.wind += current ;//* 1.01 * GetRandomMeasurement();;
        for (int k=0; k< average_number ; k++ ) m.temp += - 2 * m.wind  + 0.01 *GetRandomMeasurement();
        break;
    }

    for (int n=0;n < average_number; n++){
        force->read();
        alpha->read();
        beta->read();

        m.alpha += alpha->angle;
        m.beta += beta->angle;
        for (int k=0; k < NUMCHANNELS; k++ ){
            m.force[k] += force->dvm_si[k];
        }
    }

    // Divide by N
    m.alpha = m.alpha / average_number;
    m.beta = m.beta  / average_number;
    for (int k=0; k<6; k++){
        m.force[k] = m.force[k] / average_number;
    }
    m.temp = m.temp / average_number;
    m.wind = m.wind / average_number;
}



void MeasureThread::read_m_debug(void){

    clear_m();

    m.tempo = timer.elapsed()/1000.0;
    qDebug() << m.tempo;
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
}

void MeasureThread::stop()
{
    m_stop = true;
}

void MeasureThread::subtract(measure *minuend, measure subtrahend){
    minuend->force[0] -= subtrahend.force[0];
    minuend->force[1] -= subtrahend.force[1];
    minuend->force[2] -= subtrahend.force[2];
    minuend->force[3] -= subtrahend.force[3];
    minuend->force[4] -= subtrahend.force[4];
    minuend->force[5] -= subtrahend.force[5];
}

void MeasureThread::clear_m(void){
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
}
