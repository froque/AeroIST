#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "variable.h"
#include <QList>

class PluginManager
{
public:
    PluginManager();
    QList<VariableMeta*> getListVariableMeta(void);
    void destroyListVariableMeta(QList<VariableMeta*>);
    QList<VariablePreferences*> getListVariablePreferences(void);
    void destroyListVariablePreferences(QList<VariablePreferences*>);
    QList<VariableModel*> getListVariableModel(void);
    void destroyListVariableModel(QList<VariableModel*>);
    QList<VariableHardware*> getListVariableHardware(QList<VariableModel*> model_list);
    void destroyListVariableHardware(QList<VariableHardware*>);
private:
    QList<Factory*> fact_list;
};

#endif // PLUGINMANAGER_H
