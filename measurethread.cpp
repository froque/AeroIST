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
    m_stop = false;
    m_parent_thread = thread();
    isZero = false;

    for (int k=0; k< NUMCHANNELS;k++){
        zero.force[k] = measurement->zero[k];
    }
    QSettings settings;
    virtual_measures = settings.value(SETTINGS_VIRTUAL_MEASURES,false).toBool();
    settings.setValue(SETTINGS_VIRTUAL_MEASURES,virtual_measures);
    if (!virtual_measures){
        force       = new Force(measurement->matrix,measurement->dvm_time,zero.force);
        alpha       = new Alpha;
        beta        = new Beta;
        temperature = new Temperature;
        motor       = new Motor;
        wind        = new Wind;
    } else {
        force       = new Virtual_Force;
        alpha       = new Virtual_Alpha;
        beta        = new Virtual_Beta;
        temperature = new Virtual_Temperature;
        motor       = new Virtual_Motor;
        wind        = new Virtual_Wind;
    }
    variables.append(force);
    variables.append(alpha);
    variables.append(beta);
    variables.append(temperature);
    variables.append(motor);
    variables.append(wind);

}

MeasureThread::MeasureThread(ZeroModel *measurement,QObject *parent) :
    QObject(parent),
    average_number(measurement->average_number),
    set_alpha(measurement->set_alpha),
    set_beta(measurement->set_beta),
    set_motor(measurement->set_motor)
{
    m_stop = false;
    m_parent_thread = thread();
    isZero = true;
    n=1;
    control_type = NONE;
    QSettings settings;
    virtual_measures = settings.value(SETTINGS_VIRTUAL_MEASURES,false).toBool();
    settings.setValue(SETTINGS_VIRTUAL_MEASURES,virtual_measures);

    if (!virtual_measures){
        force       = new Force(measurement->matrix,measurement->dvm_time);
        alpha       = new Alpha;
        beta        = new Beta;
        temperature = new Temperature;
        motor       = new Motor;
        wind        = new Wind;
    } else {
        force       = new Virtual_Force;
        alpha       = new Virtual_Alpha;
        beta        = new Virtual_Beta;
        temperature = new Virtual_Temperature;
        motor       = new Virtual_Motor;
        wind        = new Virtual_Wind;
    }
    variables.append(force);
    variables.append(alpha);
    variables.append(beta);
    variables.append(temperature);
    variables.append(motor);
    variables.append(wind);

    settling_time = 0;
    start         = 0;
    end           = 0;
    step          = 0;
    current       = 0;
}

MeasureThread::~MeasureThread(){
    qDeleteAll(variables);
}

void MeasureThread::isReady(void){
//        if (motor->isReady() == false){
//            throw std::runtime_error("Motor is not ready. Try press the green button");
//        }
    Variable *variable;
    foreach (variable, variables) {
        if (variable->isReady() == false){
            throw std::runtime_error("Something is not ready.");
        }

    }
}

void MeasureThread::produce(){
    int k = 1;
    timer.start();
    QEventLoop eloop;
    set_initial();
    while(!m_stop) {
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
        emit MeasureDone(m_hash);
        eloop.processEvents(QEventLoop::AllEvents, 50);
    }

    // cleanup
    Variable *variable;
    foreach (variable, variables) {
        if (variable->is_controlable() && variable->has_set_final()){
            variable->set_final();
        }
    }

    if (m_parent_thread != thread()){
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
        alpha->set_value (0, this->set_alpha );
        beta->set_value  (0, this->set_beta  );
        motor->set_value (0, this->set_motor );
        break;
    case ALPHA:
        alpha->set_value (0, this->start     );
        beta->set_value  (0, this->set_beta  );
        motor->set_value (0, this->set_motor );
        break;
    case BETA:
        alpha->set_value (0, this->set_alpha );
        beta->set_value  (0, this->start     );
        motor->set_value (0, this->set_motor );
        break;
    case MOTOR:
        alpha->set_value (0, this->set_alpha );
        beta->set_value  (0, this->set_beta  );
        motor->set_value (0, this->start     );
        break;
    }
    Helper::msleep(settling_time*1000);
}

void MeasureThread::set_m(void){
    switch (control_type){
    case NONE :
        break;
    case ALPHA:
        alpha->set_value(0,current);
        Helper::msleep(settling_time*1000);
        break;
    case BETA:
        beta->set_value(0,current);
        Helper::msleep(settling_time*1000);
        break;
    case MOTOR:
        motor->set_value(0,current);
        Helper::msleep(settling_time*1000);
        break;
    }
}

void MeasureThread::read_m(void){

    m.tempo = timer.elapsed()/1000.0;

    QHash<QString,double> value_h;

    Variable *variable;
    // clear values
    foreach (variable, variables) {
        for (int k=0 ; k< variable->get_num(); k++){
            value_h[variable->get_name(k)] = 0;
        }
    }

    for (int n=0;n < average_number; n++){
        foreach (variable, variables) {
            variable->read();

            // get the result and add it
            for (int k=0 ; k< variable->get_num(); k++){
                value_h[variable->get_name(k)] += variable->get_value(k);
            }
        }
    }

    // Divide by N
    QHashIterator<QString, double> i(value_h);
     while (i.hasNext()) {
         i.next();
         value_h[i.key()] = i.value() / average_number;
     }

    m_hash = value_h;
    m_hash["Time"] = m.tempo;
}

//void MeasureThread::set_m_virtual(void){
//    switch (control_type){
//    case NONE :
//        break;
//    case ALPHA:
//        m.alpha = current;
//        Helper::msleep(settling_time*1000);
//        break;
//    case BETA:
//        m.beta = current;
//        Helper::msleep(settling_time*1000);
//        break;
//    case MOTOR:
//        m.motor = current;
//        Helper::msleep(settling_time*1000);
//        break;
//    }
//}


void MeasureThread::stop(){
    m_stop = true;
}


void MeasureThread::control_alpha(double angle){
    alpha->set_value(0,angle);
}

void MeasureThread::control_beta(double angle){
    beta->set_value(0,angle);
}

void MeasureThread::control_motor(double speed){
    motor->set_value(0,speed);
}
