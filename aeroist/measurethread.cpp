#include "measurethread.h"

#include <QEventLoop>
#include <QSettings>
#include <stdexcept>
#include "helper.h"

#include <QCoreApplication>
#include "pluginmanager.h"

MeasureThread::MeasureThread(MeasurementsModel *measurement,QObject *parent) :
    QObject(parent),
    measures_per_iteration(measurement->measures_per_iteration),
    settling_time(measurement->settling_time),
    end(measurement->end),
    step(measurement->step),
    control(measurement->control),
    iterations(measurement->iterations)
{
    m_stop = false;
    m_parent_thread = thread();

    PluginManager manager;
    variables = manager.getListVariableHardware(measurement->variables,true);

    foreach (VariableModel *model, measurement->variables) {
        for (int k=0; k< model->meta->get_num(); k++){
            if (control == model->meta->get_name(k)){
                current = model->start.value(k);
                start = current;
            }
        }
    }

}

MeasureThread::MeasureThread(ReferenceModel *measurement,QObject *parent) :
    QObject(parent),
    measures_per_iteration(measurement->measures_per_iteration)
{
    m_stop = false;
    m_parent_thread = thread();
        iterations=1;

    PluginManager manager;
    variables = manager.getListVariableHardware(measurement->variables,false);

    settling_time = 0;
    end           = 0;
    step          = 0;
    current       = 0;

    control = "";
}

MeasureThread::~MeasureThread(){
    PluginManager manager;
    manager.destroyListVariableHardware(variables);
}

void MeasureThread::isReady(void){
    VariableHardware *variable;
    foreach (variable, variables) {
        if (variable->isReady() == false){
            QString error_msg = QString(tr("%1 is not ready.")).arg(variable->meta->get_general_name_tr());
            throw std::runtime_error(error_msg.toStdString());
        }
    }
}

void MeasureThread::produce(){
    int k = 0;
    QEventLoop eloop;
    //clear percentage;
    emit progress( 0 );
    try{
        set_initial();
        while(!m_stop) {
            set_m();
            Helper::msleep(settling_time*1000);

            // make N reads and send them to
            for (int l =0; l< measures_per_iteration; l++){
                read_m();
                emit MeasureDone(m_hash,raw_hash);
                eloop.processEvents(QEventLoop::AllEvents, 50);

                if(control == ""){
                    if (iterations != 0 ){
                        double perc_B = (((k)*1.0)/(iterations*1.0)) ;
                        double perc_S = ((l+1) *1.0) /(measures_per_iteration *1.0);
                        double perc_T = perc_B + 1.0/(iterations*1.0) * perc_S;
                        emit progress( (int) (perc_T*100));
                    }
                } else {
                    double range = (end - start)*1.0 + step;
                    double perc_B = ((current - start) *1.0)/range;
                    double perc_S = ((l+1) *1.0) /(measures_per_iteration *1.0);
                    double perc_T = perc_B + (step *1.0)/range * perc_S;
                    emit progress( (int) (perc_T*100));
                }
            }
            current = current + step;
            k++;

            if(control == ""){
                if (iterations != 0 ){
                    if ( k>= iterations ){
                        m_stop = true;
                    }
                }
            } else {
                if (( step > 0 && current > end) || (step < 0 && current < end)){
                    m_stop = true;
                    emit progress( 100 ); // the step may not be adjusted to finish with 100%
                }
            }
            eloop.processEvents(QEventLoop::AllEvents, 50);
        }

        // set variables to final safe values

        foreach (VariableHardware *hardware, variables) {
            VariableMeta *var = hardware->meta;
            if (var->is_controlable() && hardware->has_set_final()){
                hardware->set_final();
            }
        }
    }
    catch(const std::runtime_error &err){
        // something has gone wrong in the middle of setting and reading
        // cleanup hardware variables, send a message and quit.
        QString  error(QObject::tr("There was a problem in setting or reading from hardware.\nThe measurement has stopped.\n\n"));
        error.append(err.what());
        message(error);
        if (m_parent_thread != thread()){
            thread()->quit();
            moveToThread(m_parent_thread);
            return;
        }
    }
    if (m_parent_thread != thread()){
        thread()->quit();
        moveToThread(m_parent_thread);
    }
}

void MeasureThread::set_initial(){

    foreach (VariableHardware *var, variables) {
        if (var->meta->is_controlable()){
            for (int k = 0 ; k< var->meta->get_num(); k++){
                var->set_value(k,var->start.value(k));
            }
        }
    }
}

void MeasureThread::set_m(void){
    VariableHardware *var;
    foreach (var, variables) {
        if (var->meta->is_controlable()){
            for (int k = 0 ; k< var->meta->get_num(); k++){
                if (control == var->meta->get_name(k)){
                    var->set_value(k,current);
                    return ;
                }
            }
        }
    }
}

void MeasureThread::read_m(void){
    QHash<QString,double> value_h,values_raw;
    foreach (VariableHardware *var, variables) {
        var->read();
        for (int k=0 ; k< var->meta->get_num(); k++){
            value_h[var->meta->get_name(k)] = var->get_value(k);
            values_raw[var->meta->get_name(k)] = var->get_raw_value(k);
        }
    }
    m_hash = value_h;
    raw_hash = values_raw;
}


void MeasureThread::stop(){
    m_stop = true;
}

void MeasureThread::manual_control(QHash<QString, double> hash){

    foreach (VariableHardware *var, variables) {
        if (var->meta->is_controlable()){
            for (int k = 0 ; k< var->meta->get_num(); k++){
                if(hash.contains(var->meta->get_name(k))){
                    var->set_value(k,hash[var->meta->get_name(k)]);
                }
            }
        }
    }
}
