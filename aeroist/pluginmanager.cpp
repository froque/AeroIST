#include "pluginmanager.h"

#include <QApplication>
#include <QDir>
#include <QPluginLoader>
#include "variable.h"
#include "data.h"


PluginManager::PluginManager(){
    Factory *factory;
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        factory = qobject_cast<Factory*> (loader.instance());
        if(factory){
            fact_list.append(factory);
        }
    }
}

QList<VariableMeta*> PluginManager::getListVariableMeta(void){
    QList<VariableMeta*> variables;
    foreach (Factory *factory, fact_list ) {
        VariableMeta * meta = factory->CreateVariableMeta();
        if (meta != NULL){
            variables.append( meta);
        }
    }
    return variables;
}

void PluginManager::destroyListVariableMeta(QList<VariableMeta*> variables){
    qDeleteAll(variables);
}

QList<VariablePreferences*>  PluginManager::getListVariablePreferences(void){
    QList<VariablePreferences*> variables;
    foreach (Factory *factory, fact_list ) {
        VariablePreferences * pref = factory->CreateVariableGUI();
        if (pref != NULL){
            pref->meta = factory->CreateVariableMeta();
            variables.append( pref);
        }
    }
    return variables;
}

void PluginManager::destroyListVariablePreferences(QList<VariablePreferences*> variables){
    foreach (VariablePreferences *var, variables) {
        delete var->meta;
    }
    qDeleteAll(variables);
}

QList<VariableModel*> PluginManager::getListVariableModel(void){
    QList<VariableModel*> variables;
    foreach (Factory *factory, fact_list ) {
        VariableModel *model = factory->CreateVariableModel();
        if (model != NULL){
            model->meta = factory->CreateVariableMeta();
            model->data = new Data(model->meta->get_num());
            variables.append(model);
        }
    }
    return variables;
}

void PluginManager::destroyListVariableModel(QList<VariableModel*> variables){
    foreach (VariableModel *var, variables) {
        delete var->meta;
        delete var->data;
    }
    qDeleteAll(variables);
}

QList<VariableHardware*> PluginManager::getListVariableHardware(QList<VariableModel*> model_list){
    QList<VariableHardware*> variables;
    foreach (Factory *factory, fact_list ) {

        //create a meta to match the list
        VariableMeta *meta = factory->CreateVariableMeta();
        if (meta != NULL){
            foreach (VariableModel *model, model_list) {
                if (meta->get_general_name() == model->meta->get_general_name()){
                    VariableHardware *hardware;
                    try {
                        hardware = factory->CreateVariableHardware(model);
                        hardware->meta = factory->CreateVariableMeta();
                        hardware->start = model->start;
                        if(model->meta->has_zero()){
                            if (model->meta->get_general_name() == hardware->meta->get_general_name()){
                                hardware->set_zero(model->data->get_zero());
                            }
                        }
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
            delete meta;
        }
    }
    return variables;
}
void PluginManager::destroyListVariableHardware(QList<VariableHardware*> variables){
    foreach (VariableHardware *var, variables) {
        delete var->meta;
    }
    qDeleteAll(variables);
}
