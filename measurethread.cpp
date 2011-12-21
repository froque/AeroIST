#include "measurethread.h"

#include <cstdlib> // for RAND_MAX

#include <QEventLoop>
#include <QDebug>
#include <QSettings>
#include <stdexcept>
#include "helper.h"

MeasureThread::MeasureThread(MeasurementsModel *measurement,QObject *parent) :
    QObject(parent),
    average_number(measurement->average_number),
    settling_time(measurement->settling_time),
    start(measurement->start),
    end(measurement->end),
    step(measurement->step),
    current(measurement->start),
    set_alpha(measurement->set_alpha),
    set_beta(measurement->set_beta),
    set_motor(measurement->set_motor),
    control_type(measurement->control_type),
    n(measurement->n)
{
    m_parent_thread = thread();
    isZero = false;

    for (int k=0; k< NUMCHANNELS;k++){
        zero.force[k] = measurement->zero[k];
    }
    QSettings settings;
    virtual_measures = settings.value(SETTINGS_VIRTUAL_MEASURES,false).toBool();
    settings.setValue(SETTINGS_VIRTUAL_MEASURES,virtual_measures);
    if (!virtual_measures){
        force = new Force(measurement->matrix,measurement->dvm_time,zero.force);
        alpha = new Alpha;
        beta = new Beta;
        temperature = new Temperature;
        motor = new Motor;
        wind = new Wind;
    }
}

MeasureThread::MeasureThread(ZeroModel *measurement,QObject *parent) :
    QObject(parent),
    average_number(measurement->average_number),
    set_alpha(measurement->set_alpha),
    set_beta(measurement->set_beta),
    set_motor(measurement->set_motor)
{
    m_parent_thread = thread();
    isZero = true;
    n=1;
    control_type = NONE;
    QSettings settings;
    virtual_measures = settings.value(SETTINGS_VIRTUAL_MEASURES,false).toBool();
    settings.setValue(SETTINGS_VIRTUAL_MEASURES,virtual_measures);
    if (!virtual_measures){
        force = new Force(measurement->matrix,measurement->dvm_time);
        alpha = new Alpha;
        beta = new Beta;
        temperature = new Temperature;
        motor = new Motor;
        wind = new Wind;
    }
}


MeasureThread::~MeasureThread(){
    if (!virtual_measures){
        delete force;
        delete alpha;
        delete beta;
        delete temperature;
        delete motor;
        delete wind;
    }
}

void MeasureThread::isReady(void){
    if (!virtual_measures){
        if (motor->isReady() == false){
            throw std::runtime_error("Motor is not ready. Try press the green button");
        }
        if (force->isReady() == false){
            throw std::runtime_error("Error with GPIB multimeter");
        }
    }
}

void MeasureThread::produce(){
    m_stop = false;
    timer.start();
    QEventLoop eloop;
    k = 1;
    if (!virtual_measures){
        set_initial();
        while(!m_stop) {
            clear_m();
            set_m();
            read_m();
            if (control_type == NONE){
                if (n != 0 && k>= n ){
                    m_stop = true;
                }
            } else {
                current = current + step;
                if ((start < end && current > end ) || (start > end && current < end)){
                    m_stop = true;
                }
            }

            k++;
            emit MeasureDone(m);
            eloop.processEvents(QEventLoop::AllEvents, 50);
        }
        // cleanup
        motor->set(0);

    } else {
        while(!m_stop) {
            clear_m();
            read_m_virtual();
            set_m_virtual();

            if (control_type == NONE){
                if (n != 0 && k>= n ){
                    m_stop = true;
                }
            } else {
                current = current + step;
                if ((start < end && current > end ) || (start > end && current < end)){
                    m_stop = true;
                }
            }

            k++;
            emit MeasureDone(m);

            eloop.processEvents(QEventLoop::AllEvents, 50);
        }
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

void MeasureThread::set_initial(void){
    switch (control_type){
    case NONE :
        alpha->set(this->set_alpha);
        beta->set(this->set_beta);
        motor->set(this->set_motor);
        break;
    case ALPHA:
        alpha->set(this->start);
        beta->set(this->set_beta);
        motor->set(this->set_motor);
        break;
    case BETA:
        alpha->set(this->set_alpha);
        beta->set(this->start);
        motor->set(this->set_motor);
        break;
    case MOTOR:
        alpha->set(this->set_alpha);
        beta->set(this->set_beta);
        motor->set(this->start);
        break;
    }
    Helper::msleep(settling_time*1000);
}

void MeasureThread::set_m(void){
    switch (control_type){
    case NONE :
        break;
    case ALPHA:
        alpha->set(current);
        Helper::msleep(settling_time*1000);
        break;
    case BETA:
        beta->set(current);
        Helper::msleep(settling_time*1000);
        break;
    case MOTOR:
        motor->set(current);
        Helper::msleep(settling_time*1000);
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
        motor->read();
        wind->read();
        m.alpha += alpha->angle;
        m.beta += beta->angle;
        m.temp += temperature->temp;
        m.motor += motor->speed_actual;
        m.wind += wind->wind;
        for (int k=0; k < NUMCHANNELS; k++ ){
            m.force[k] += force->forces[k];
        }
    }

    // Divide by N
    m.alpha = m.alpha / average_number;
    m.beta = m.beta  / average_number;
    for (int k=0; k<6; k++){
        m.force[k] = m.force[k] / average_number;
    }
    m.temp = m.temp / average_number;
    m.motor = m.motor / average_number;
    m.wind = m.wind / average_number;
}

void MeasureThread::set_m_virtual(void){
    switch (control_type){
    case NONE :
        break;
    case ALPHA:
        m.alpha = current;
        Helper::msleep(settling_time*1000);
        break;
    case BETA:
        m.beta = current;
        Helper::msleep(settling_time*1000);
        break;
    case MOTOR:
        m.motor = current;
        Helper::msleep(settling_time*1000);
        break;
    }
}

void MeasureThread::read_m_virtual(void){

    m.tempo = timer.elapsed()/1000.0;

    for (int n=0;n < average_number; n++){
        m.alpha += GetRandomMeasurement();
        m.beta += GetRandomMeasurement();
        m.temp += GetRandomMeasurement();
        m.motor += GetRandomMeasurement();
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
    m.motor = m.motor / average_number;
    m.wind = m.wind / average_number;
    Helper::msleep( 0.5 * average_number *  1000.0 * qrand() / RAND_MAX);
}

void MeasureThread::stop(){
    m_stop = true;
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
    m.motor=0;
    m.wind=0;
}

void MeasureThread::control_alpha(double angle){
    alpha->set(angle);
}

void MeasureThread::control_beta(double angle){
    beta->set(angle);
}

void MeasureThread::control_motor(double speed){
    motor->set(speed);
}
