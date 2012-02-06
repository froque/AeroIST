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
        variables.append(new Force(measurement->matrix,measurement->dvm_time,zero.force));
        variables.append(new Alpha);
        variables.append(new Beta);
        variables.append(new Temperature);
        variables.append(new Motor);
        variables.append(new Wind);

    } else {
        variables.append(new Virtual_Force);
        variables.append(new Virtual_Alpha);
        variables.append(new Virtual_Beta);
        variables.append(new Virtual_Temperature);
        variables.append(new Virtual_Motor);
        variables.append(new Virtual_Wind);
    }
    switch(measurement->control_type){
    case NONE :
        control = "";
        break;
    case ALPHA:
        control = "Alpha";
        break;
    case BETA:
        control = "Beta";
        break;
    case MOTOR:
        control = "Motor";
        break;
    }
    start_hash["Alpha"] = measurement->set_alpha;
    start_hash["Beta"] = measurement->set_beta;
    start_hash["Motor"] = measurement->set_motor;
}

MeasureThread::MeasureThread(ZeroModel *measurement,QObject *parent) :
    QObject(parent),
    average_number(measurement->average_number)
{
    m_stop = false;
    m_parent_thread = thread();
    isZero = true;
    n=1;
    QSettings settings;
    virtual_measures = settings.value(SETTINGS_VIRTUAL_MEASURES,false).toBool();
    settings.setValue(SETTINGS_VIRTUAL_MEASURES,virtual_measures);

    if (!virtual_measures){
        variables.append(new Force(measurement->matrix,measurement->dvm_time));
        variables.append(new Alpha);
        variables.append(new Beta);
        variables.append(new Temperature);
        variables.append(new Motor);
        variables.append(new Wind);

    } else {
        variables.append(new Virtual_Force);
        variables.append(new Virtual_Alpha);
        variables.append(new Virtual_Beta);
        variables.append(new Virtual_Temperature);
        variables.append(new Virtual_Motor);
        variables.append(new Virtual_Wind);
    }

    settling_time = 0;
    start         = 0;
    end           = 0;
    step          = 0;
    current       = 0;

    control = "";
    start_hash["Alpha"] = measurement->set_alpha;
    start_hash["Beta"] = measurement->set_beta;
    start_hash["Motor"] = measurement->set_motor;
}

MeasureThread::~MeasureThread(){
    qDeleteAll(variables);
}

void MeasureThread::isReady(void){
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
        Helper::msleep(settling_time*1000);
        read_m();
        if(control == ""){
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

    // set variables to final safe values
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

void MeasureThread::set_initial(){
    Variable *var;
    foreach (var, variables) {
        if (var->is_controlable()){
            for (int k = 0 ; k< var->get_num(); k++){
                if(start_hash.contains(var->get_name(k))){
                    var->set_value(k,start_hash[var->get_name(k)]);
                }
            }
        }
    }
}

void MeasureThread::set_m(void){
    Variable *var;
    foreach (var, variables) {
        if (var->is_controlable()){
            for (int k = 0 ; k< var->get_num(); k++){
                if (control == var->get_name(k)){
                    var->set_value(k,current);
                    return ;
                }
            }
        }
    }
}

void MeasureThread::read_m(void){

    double tempo = timer.elapsed()/1000.0;

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
    m_hash["Time"] = tempo;
}


void MeasureThread::stop(){
    m_stop = true;
}

void MeasureThread::manual_control(QHash<QString, double> hash){
    Variable *var;
    foreach (var, variables) {
        if (var->is_controlable()){
            for (int k = 0 ; k< var->get_num(); k++){
                if(hash.contains(var->get_name(k))){
                    var->set_value(k,hash[var->get_name(k)]);
                }
            }
        }
    }
}
