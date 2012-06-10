#include "referencemodel.h"
#include "pluginmanager.h"
#include "data.h"

ReferenceModel::ReferenceModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    name="";
    measures_per_iteration=0;
    init();
}

ReferenceModel::ReferenceModel(QDomElement root,QObject *parent) :
    QAbstractTableModel(parent)
{
    init();
    load_xml(root);
}

ReferenceModel::~ReferenceModel(){
    PluginManager manager;
    manager.destroyListVariableModel(variables);
}

void ReferenceModel::init(){
    PluginManager manager;
    variables = manager.getListVariableModel();
}

int ReferenceModel::rowCount(const QModelIndex &parent) const{
    Q_UNUSED(parent);
    if (variables.isEmpty()){
        return 0;
    } else {
        return variables.first()->data->get_size();
    }
}

int ReferenceModel::columnCount(const QModelIndex &parent) const{
    Q_UNUSED(parent);
    int size = 0;
    foreach (VariableModel *var, variables) {
        if (var->meta->has_zero()){
            size += var->meta->get_num();
        }
    }
    return size;
}

QVariant ReferenceModel::data(const QModelIndex &index, int role) const{
    if (!index.isValid()){
        return QVariant();
    }
    // Qt::DisplayRole - show normal values
    // Qt::UserRole    - show raw values
    if (role == Qt::DisplayRole) {
        int row = index.row();
        int upper = 0,lower = 0;
        int column = index.column();
        foreach (VariableModel *var, variables) {
            if(var->meta->has_zero()){
                lower = upper;
                upper += var->meta->get_num();
                if ( column < upper){
                    return var->data->get_value( column- lower , row);
                }
            }
        }
    }
    if (role == Qt::UserRole) {
        int row = index.row();
        int upper = 0,lower = 0;
        int column = index.column();
        foreach (VariableModel *var, variables) {
            if(var->meta->has_zero()){
                lower = upper;
                upper += var->meta->get_num();
                if ( column < upper){
                    return var->data->get_raw_value( column- lower , row);
                }
            }
        }
    }
    return QVariant();
}

QVariant ReferenceModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if ( !( (role == Qt::DisplayRole) || (role >= Qt::UserRole) )){
        return QVariant();
    }

    if (orientation == Qt::Vertical){
        return section+1;
    }
    // Qt::DisplayRole - don't show units
    // Qt::UserRole    - dont't translate
    // Qt::UserRole +1 - translate and show units
    // Qt::UserRole +2 - translate and show raw units
    if (orientation == Qt::Horizontal) {
        int upper = 0, lower = 0;
        int column = section;
        foreach (VariableModel *var, variables) {
            if (var->meta->has_zero()){
                lower = upper;
                upper += var->meta->get_num();
                if ( column < upper){
                    switch (role){
                    case Qt::DisplayRole: return var->meta->get_name_tr( column- lower ); break;
                    case Qt::UserRole +1: return var->meta->get_name_tr(column- lower).append(" (").append(var->meta->get_units(column- lower)).append(")"); break;
                    case Qt::UserRole +2: return var->meta->get_name_tr(column- lower).append(" (").append(var->meta->get_raw_units(column- lower)).append(")"); break;
                    case Qt::UserRole: return var->meta->get_name( column- lower ); break;
                    }
                }
            }
        }
    }
    return QVariant();
}

void ReferenceModel::GetMeasure(QHash<QString,double> hash, QHash<QString, double> raw_hash){
    beginInsertRows(QModelIndex(), rowCount(),rowCount());
    foreach (VariableModel *var, variables) {
        for (int k=0; k< var->meta->get_num(); k++){
            if(hash.contains(var->meta->get_name(k))){
                var->data->append_value(k,hash[var->meta->get_name(k)]);
                var->data->append_raw_value(k,raw_hash[var->meta->get_name(k)]);
            }
        }
    }
    endInsertRows();
}

void ReferenceModel::save_xml(QDomElement root){
    QDomElement name = root.ownerDocument().createElement(TAG_NAME);
    name.appendChild(root.ownerDocument().createTextNode(this->name));
    root.appendChild(name);

    QDomElement description = root.ownerDocument().createElement(TAG_DESCRIPTION);
    description.appendChild(root.ownerDocument().createTextNode(this->description));
    root.appendChild(description);

    QDomElement options = root.ownerDocument().createElement(TAG_OPTIONS);
    root.appendChild(options);
    foreach (VariableModel *var, variables) {
        var->save_xml(options);
    }

    QDomElement meas_it_element = root.ownerDocument().createElement(TAG_MEASURES_PER_ITERATION);
    meas_it_element.appendChild(root.ownerDocument().createTextNode(QString::number(this->measures_per_iteration)));
    root.appendChild(meas_it_element);

    QDomElement start_hash_element = root.ownerDocument().createElement(TAG_START_VALUES);
    root.appendChild(start_hash_element);
    QDomElement item = root.ownerDocument().createElement(TAG_ITEM);
    start_hash_element.appendChild(item);
    QDomElement start_element;
    foreach (VariableModel *var, variables) {
        if (var->meta->is_controlable()){
            for (int k = 0; k< var->meta->get_num(); k++){
                start_element = root.ownerDocument().createElement(var->meta->get_name(k));
                start_element.appendChild( root.ownerDocument().createTextNode(QString::number( var->start.at(k) ,'g',10)));
                item.appendChild(start_element);
            }
        }
    }


    QDomElement data_element = root.ownerDocument().createElement(TAG_DATA);
    root.appendChild(data_element);

    QDomElement force;
    QString tag_header;
    QDomElement element;
    QString data;

    for (int row=0; row < rowCount(); row++){
        element = root.ownerDocument().createElement(TAG_ITEM);
        data_element.appendChild(element);
        for (int column =0; column < columnCount(); column++){
            tag_header = this->headerData(column,Qt::Horizontal,Qt::UserRole).toString().simplified();
            tag_header.replace(" ","_");
            force = root.ownerDocument().createElement(tag_header);
            data = this->data(this->index(row,column)).toString();
            force.appendChild( root.ownerDocument().createTextNode(data));
            element.appendChild(force);
        }
    }

    QDomElement raw_data_element = root.ownerDocument().createElement(TAG_RAW_DATA);
    root.appendChild(raw_data_element);
    for (int row=0; row < rowCount(); row++){
        element = root.ownerDocument().createElement(TAG_ITEM);
        raw_data_element.appendChild(element);
        for (int column =0; column < columnCount(); column++){
            tag_header = this->headerData(column,Qt::Horizontal,Qt::UserRole).toString().simplified();
            tag_header.replace(" ","_");
            force = root.ownerDocument().createElement(tag_header);
            data = this->data(this->index(row,column),Qt::UserRole).toString();
            force.appendChild( root.ownerDocument().createTextNode(data));
            element.appendChild(force);
        }
    }
}

void ReferenceModel::load_xml(QDomElement root){

    QDomNodeList nodelist = root.childNodes();
    QDomNode node;
    QDomElement element;
    for (int k=0; k< nodelist.count();k++){
        node = nodelist.at(k);
        element = node.toElement();
        if (element.tagName() == TAG_NAME){
            this->name = element.text();
            continue;
        }
        if (element.tagName() == TAG_DESCRIPTION){
            this->description = element.text();
            continue;
        }
        if (element.tagName() == TAG_OPTIONS){
            foreach (VariableModel *var, variables) {
                var->load_xml(element);
            }
            continue;
        }

        if (element.tagName() == TAG_MEASURES_PER_ITERATION){
            this->measures_per_iteration = element.text().toInt();
            continue;
        }
        if (element.tagName() == TAG_START_VALUES){
            QDomNodeList items = element.childNodes();
            QDomElement item;

            // cycle the items list
            for (int k = 0; k < items.count(); k++){
                item = items.at(k).toElement();
                if (item.tagName() == TAG_ITEM){
                    QDomNodeList node_vars = item.childNodes();
                    QDomElement node_var;

                    // cycle the variables model list
                    foreach (VariableModel *var, variables) {
                        if(var->meta->is_controlable()){
                            var->start = QVector<double>(var->meta->get_num());

                            // cycle the variables file list
                            for (int n = 0; n < node_vars.count(); n++ ){
                                node_var = node_vars.at(n).toElement();

                                // cycle each variable dimensions
                                for (int n=0; n<var->meta->get_num(); n++){
                                    if (node_var.nodeName() == var->meta->get_name(n)){
                                        var->start[n] = node_var.text().toDouble();
                                    }
                                }
                            }
                        }
                    }
                }
            }
            continue;
        }
        if (element.tagName() == TAG_DATA){
            QDomNodeList items = element.childNodes();
            QDomElement item;
            for (int row = 0; row < items.count(); row++){
                insertRow(rowCount());
                item = items.at(row).toElement();
                QDomNodeList forces = item.childNodes();
                QDomElement force;
                for (int column = 0; column< forces.count(); column++ ){
                    force = forces.at(column).toElement();
                    QString tagname = force.tagName();
                    QString tag_header;
                    for (int k=0; k< columnCount(); k++){
                        tag_header = this->headerData(k,Qt::Horizontal,Qt::UserRole).toString().simplified();
                        tag_header.replace(" ","_");
                        if (tagname == tag_header){
                            this->setData(index(row,k),force.text());
                            break;
                        }
                    }
                }
            }
            continue;
        }
        if (element.tagName() == TAG_RAW_DATA){
            QDomNodeList items = element.childNodes();
            QDomElement item;
            for (int row = 0; row < items.count(); row++){
                // the previous if has already used insertrows that works for here
                item = items.at(row).toElement();
                QDomNodeList forces = item.childNodes();
                QDomElement force;
                for (int column = 0; column< forces.count(); column++ ){
                    force = forces.at(column).toElement();
                    QString tagname = force.tagName();
                    QString tag_header;
                    for (int k=0; k< columnCount(); k++){
                        tag_header = this->headerData(k,Qt::Horizontal,Qt::UserRole).toString().simplified();
                        tag_header.replace(" ","_");
                        if (tagname == tag_header){
                            this->setData(index(row,k),force.text(),Qt::UserRole);
                            break;
                        }
                    }
                }
            }
            continue;
        }
    }
}

bool ReferenceModel::setData ( const QModelIndex & index, const QVariant & value, int role){
    if (!index.isValid()){
        return false;
    }
    if (role == Qt::EditRole || role == Qt::UserRole) {
        int row = index.row();
        if (rowCount() < row ){
            return false;
        }
        int upper = 0, lower = 0;
        int column = index.column();
        foreach (VariableModel *var, variables) {
            if (var->meta->has_zero()){
                lower = upper;
                upper += var->meta->get_num();
                if ( column < upper){
                    if (role== Qt::EditRole){
                        var->data->set_value(column - lower,row,value.toDouble());
                    } else {
                        var->data->set_raw_value(column - lower,row,value.toDouble());
                    }
                    return true;
                }
            }
        }
    }
    return true;
}

bool ReferenceModel::insertRows ( int row, int count, const QModelIndex & parent ){
    Q_UNUSED(parent)
    if (row < 0 ){
        return false;
    }

    foreach (VariableModel *var, variables) {
        for (int k=0; k< var->meta->get_num(); k++){
            var->data->insert_value(k, row, count, 0);
            var->data->insert_raw_value(k,row,count,0);
        }
    }
    return true;
}
bool ReferenceModel::insertRow ( int row,  const QModelIndex & parent ){
    return insertRows(row,1,parent);
}
