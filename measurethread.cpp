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

    for (int k=0; k< NUMCHANNELS;k++){
        zero.force[k] = measurement->zero->force[k].first();
    }
#if REAL_MEASURES
    force = new Force(measurement->matrix,measurement->dvm_time);
    alpha = new Alpha;
    beta = new Beta;
    temperature = new Temperature;
    wind = new Wind;
#endif //REAL_MEASURES
}

MeasureThread::MeasureThread(ZeroModel *measurement,QObject *parent) :
    QObject(parent),
    average_number(measurement->average_number)
{
    m_parent_thread = thread();
    isZero = true;
    n=1;
    control_type = NONE;
#if REAL_MEASURES
    force = new Force(measurement->matrix,measurement->dvm_time);
    alpha = new Alpha;
    beta = new Beta;
    temperature = new Temperature;
    wind = new Wind;
#endif //REAL_MEASURES
}

MeasureThread::~MeasureThread(){
#if REAL_MEASURES
    delete force;
#endif //REAL_MEASURES
}

void MeasureThread::produce(){
    m_stop = false;
    timer.start();
    QEventLoop eloop;
    k = 1;
    while(!m_stop) {
        clear_m();
#if REAL_MEASURES
        set_m();
        read_m();
#else
        set_m_virtual();
        read_m_virtual();
#endif //REAL_MEASURES
        if (isZero == false){
            subtract(&m,zero);
        }

        if (control_type == NONE){
            if (n != 0 && k>= n ){
                m_stop = true;
            }
        } else {
            current = current + step;
            if (current > max ){
                m_stop = true;
            }
        }

        k++;
        qDebug() << "before emit signal " << m.force[0];
        emit MeasureDone(m);

        eloop.processEvents(QEventLoop::AllEvents, 50);
    }
#if REAL_MEASURES
    wind->set(0);   // cleanup
#endif //REAL_MEASURES
    if (m_parent_thread != thread())
    {
        thread()->quit();
        moveToThread(m_parent_thread);
    }
}

double MeasureThread::GetRandomMeasurement(void){
    return 10.0 * qrand() / RAND_MAX;
}


void MeasureThread::set_m(void){
    switch (control_type){
    case NONE :
        break;
    case ALPHA:
        Helper::msleep(settling_time*1000);
        alpha->set(current);
        break;
    case BETA:
        Helper::msleep(settling_time*1000);
        beta->set(current);
        break;
    case WIND:
        Helper::msleep(settling_time*1000);
        wind->set(current);
        break;
    }
}

void MeasureThread::read_m(void){

    m.tempo = timer.elapsed()/1000.0;

    for (int n=0;n < average_number; n++){
        force->read();
        alpha->read();
        beta->read();
        temperature->read();
        wind->get();
        m.alpha += alpha->angle;
        m.beta += beta->angle;
        m.temp += temperature->temp;
        m.wind += wind->speed_actual;
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

void MeasureThread::set_m_virtual(void){
    switch (control_type){
    case NONE :
        break;
    case ALPHA:
        Helper::msleep(settling_time*1000);
        m.alpha = current;
        break;
    case BETA:
        Helper::msleep(settling_time*1000);
        m.beta = current;
        break;
    case WIND:
        Helper::msleep(settling_time*1000);
        m.wind = current;
        break;
    }
}

void MeasureThread::read_m_virtual(void){

    m.tempo = timer.elapsed()/1000.0;

    for (int n=0;n < average_number; n++){
        m.alpha += GetRandomMeasurement();
        m.beta += GetRandomMeasurement();
        m.temp += GetRandomMeasurement();
        m.wind += GetRandomMeasurement();
        for (int k=0; k < NUMCHANNELS; k++ ){
            m.force[k] += GetRandomMeasurement();
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


void MeasureThread::read_m_virtual_orig(void){

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

void MeasureThread::set_alpha(double angle){
//    alpha->set(angle);
}

void MeasureThread::set_beta(double angle){
//    beta->set(angle);
}

void MeasureThread::set_wind(double speed){
//    wind->set(speed);
}
