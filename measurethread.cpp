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
    average_number(measurement->average_number),
    settling_time(measurement->settling_time),
    end(measurement->end),
    step(measurement->step),
    control(measurement->control),
    n(measurement->n)
{
    m_stop = false;
    m_parent_thread = thread();
    isZero = false;

//    QSettings settings;
//    virtual_measures = settings.value(SETTINGS_VIRTUAL_MEASURES,false).toBool();
//    settings.setValue(SETTINGS_VIRTUAL_MEASURES,virtual_measures);

    init(measurement->variables);

    // set zero to each variable
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
            }
        }
    }

}

MeasureThread::MeasureThread(ZeroModel *measurement,QObject *parent) :
    QObject(parent),
    average_number(measurement->average_number)
{
    m_stop = false;
    m_parent_thread = thread();
    isZero = true;
    n=1;
//    QSettings settings;
//    virtual_measures = settings.value(SETTINGS_VIRTUAL_MEASURES,false).toBool();
//    settings.setValue(SETTINGS_VIRTUAL_MEASURES,virtual_measures);

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
            foreach (VariableModel *model, list) {
                if (meta->get_general_name() == model->meta->get_general_name()){
                    VariableHardware *hardware = factory->CreateVariableHardware(model);
                    if (hardware != NULL){
                        variables.append(hardware );
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
            if (( step > 0 && current > end) || (step < 0 && current < end)){
                m_stop = true;
            }
        }

        k++;
        emit MeasureDone(m_hash);
        eloop.processEvents(QEventLoop::AllEvents, 50);
    }

    // set variables to final safe values

    foreach (VariableHardware *hardware, variables) {
        VariableMeta *var = hardware->meta;
        if (var->is_controlable() && hardware->has_set_final()){
            hardware->set_final();
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

    double tempo = timer.elapsed()/1000.0;

    QHash<QString,double> value_h;


    // clear values
    foreach (VariableHardware *var, variables) {
        for (int k=0 ; k< var->meta->get_num(); k++){
            value_h[var->meta->get_name(k)] = 0;
        }
    }

    for (int n=0;n < average_number; n++){
        foreach (VariableHardware *var, variables) {
            var->read();

            // get the result and add it
            for (int k=0 ; k< var->meta->get_num(); k++){
                value_h[var->meta->get_name(k)] += var->get_value(k);
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
