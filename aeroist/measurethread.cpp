#include "measurethread.h"

#include <QEventLoop>
#include <QSettings>
#include <stdexcept>
#include "helper.h"

#include <QDir>
#include <QCoreApplication>
#include <QPluginLoader>

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

    init(measurement->variables);

    // set references/zeros to each variable
    foreach (VariableHardware *hard_var, variables) {
        foreach (VariableModel *var, measurement->variables) {
            if(var->meta->has_zero()){
                if (var->meta->get_general_name() == hard_var->meta->get_general_name()){
                    hard_var->set_zero(var->get_zero());
                }
            }
        }
    }

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

    init(measurement->variables);

    settling_time = 0;
    end           = 0;
    step          = 0;
    current       = 0;

    control = "";
}

void MeasureThread::init(QList<VariableModel*> list){
    Factory *factory;
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        factory = qobject_cast<Factory*>( loader.instance());
        if(factory){
            VariableMeta *meta = factory->CreateVariableMeta();
            if (meta != NULL){
                foreach (VariableModel *model, list) {
                    if (meta->get_general_name() == model->meta->get_general_name()){
                        VariableHardware *hardware;
                        try {
                            hardware = factory->CreateVariableHardware(model);
                        } catch (...){
                            qDeleteAll(variables);
                            throw;
                        }
                        if (hardware != NULL){
                            variables.append(hardware );
                            break;
                        }
                    }
                }
            }
        }
    }
    foreach (VariableHardware *hard_var, variables) {
        foreach (VariableModel *var, list) {
            if(var->meta->is_controlable()){
                if (var->meta->get_general_name() == hard_var->meta->get_general_name()){
                    hard_var->start = var->start;
                }
            }
        }
    }
}

MeasureThread::~MeasureThread(){
    qDeleteAll(variables);
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
    int k = 1;
    QEventLoop eloop;
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
            }

            if(control == ""){
                if (iterations != 0 ){
                    double perc = ((k*1.0)/(iterations*1.0)) *100;
                    emit progress( (int) perc);
                    if ( k>= iterations ){
                        m_stop = true;
                    }
                }
            } else {
                current = current + step;
                double perc = ((current - start) *1.0)/((end - start)*1.0) *100;
                emit progress( (int) perc);
                if (( step > 0 && current > end) || (step < 0 && current < end)){
                    m_stop = true;
                    emit progress( 100 ); // the step may not be adjusted to finish with 100%
                }
            }

            k++;
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
        QString  error(QObject::tr("Theres was a problem in setting or reading from hardware.\nThe measurement has stopped.\n\n"));
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
