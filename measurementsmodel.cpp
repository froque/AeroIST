#include <QtGlobal>
#include <QStringList>
#include "measurementsmodel.h"

#ifdef DEBUG
#include "QDebug"
#endif // DEBUG

#include <QDir>
#include <QCoreApplication>
#include <QPluginLoader>

MeasurementsModel::MeasurementsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    name="";
    average_number=0;
    settling_time=0;
    end=0;
    step=0;
    control="";
    n = 0;
    init();
}

MeasurementsModel::MeasurementsModel(QDomElement root, QObject *parent):
    QAbstractTableModel(parent)
{
    init();
    load_xml(root);
}

void MeasurementsModel::init(){
    Factory *factory;
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        factory = qobject_cast<Factory*>( loader.instance());
        if(factory){
            variables.append( factory->CreateVariableModel());
        }
    }
}

void MeasurementsModel::save_csv(QTextStream *out,bool header){
    int rows = rowCount();
    int columns = columnCount();

    if (header == true){
        for (int column=0; column < columns; column++){
            *out << headerData(column,Qt::Horizontal,Qt::DisplayRole).toString();
            if (column != columns-1){
                *out << ";";
            }
        }
        *out << endl;
    }
    QModelIndex ind;
    for (int row =0; row < rows;row++){
        for (int column=0; column < columns; column++){
            ind = index(row,column,QModelIndex());
            *out << data(ind,Qt::DisplayRole).toDouble();
            if (column != columns-1){
                *out << ";";
            }
        }
        *out << endl;
    }
}

void MeasurementsModel::GetMeasure(QHash<QString,double> hash){

    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    foreach (VariableModel *var, variables) {
        for (int k=0; k< var->meta->get_num(); k++){
            if(hash.contains(var->meta->get_name(k))){
                var->append_value(k,hash[var->meta->get_name(k)]);
            }
        }
    }

    endInsertRows();
}

int MeasurementsModel::columnCount(const QModelIndex &parent)  const{
    Q_UNUSED(parent);
    int size = 0;
    foreach (VariableModel *var, variables) {
        size += var->meta->get_num();
    }
    return size;
}

int MeasurementsModel::rowCount(const QModelIndex &parent ) const{
    Q_UNUSED(parent);
    if (variables.isEmpty()){
        return 0;
    } else {
        return variables.first()->get_size();
    }
}

QVariant MeasurementsModel::data(const QModelIndex &index, int role) const{
    if (!index.isValid()){
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        int row = index.row();
        int upper = 0;
        int lower = 0;
        int column = index.column();
        foreach (VariableModel *var, variables) {
            lower = upper;
            upper += var->meta->get_num();
            if ( column < upper){
                return var->get_value( column- lower , row);
            }
        }
    }
    return QVariant();
}

QVariant MeasurementsModel::headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Vertical){
        return section+1;
    }

    if (orientation == Qt::Horizontal) {
        int upper = 0;
        int lower = 0;
        int column = section;
        foreach (VariableModel *var, variables) {
            lower = upper;
            upper += var->meta->get_num();
            if ( column < upper){
                return var->meta->get_name( column- lower );
            }
        }
    }
    return QVariant();
}


QVector<double>  MeasurementsModel::vector_data(int index){
    int upper = 0;
    int lower = 0;
    int column = index;
    foreach (VariableModel *var, variables) {
        lower = upper;
        upper += var->meta->get_num();
        if ( column < upper){
            return var->get_vector(column - lower);
        }
    }

    QVector<double> stupid_warning;
    return stupid_warning;
}

void MeasurementsModel::save_xml(QDomElement root ){
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

    QDomElement average_number = root.ownerDocument().createElement(TAG_AVERAGE_NUMBER);
    average_number.appendChild(root.ownerDocument().createTextNode(QString::number(this->average_number)));
    root.appendChild(average_number);

    QDomElement settling_time = root.ownerDocument().createElement(TAG_SETTLING_TIME);
    settling_time.appendChild(root.ownerDocument().createTextNode(QString::number(this->settling_time)));
    root.appendChild(settling_time);

    QDomElement end = root.ownerDocument().createElement(TAG_END);
    end.appendChild(root.ownerDocument().createTextNode(QString::number(this->end)));
    root.appendChild(end);

    QDomElement step = root.ownerDocument().createElement(TAG_STEP);
    step.appendChild(root.ownerDocument().createTextNode(QString::number(this->step)));
    root.appendChild(step);

    QDomElement start_hash_element = root.ownerDocument().createElement(TAG_START_VALUES);
    root.appendChild(start_hash_element);
    QDomElement item = root.ownerDocument().createElement(TAG_ITEM);
    start_hash_element.appendChild(item);

    QDomElement start_element;
    QHashIterator<QString, double> i(start_hash);
    while (i.hasNext()) {
        i.next();
        start_element = root.ownerDocument().createElement(i.key());
        start_element.appendChild( root.ownerDocument().createTextNode(QString::number( i.value() ,'g',10)));
        item.appendChild(start_element);
    }
    QDomElement control_type = root.ownerDocument().createElement(TAG_CONTROL_TYPE);
    control_type.appendChild(root.ownerDocument().createTextNode(this->control));
    root.appendChild(control_type);

    QDomElement n = root.ownerDocument().createElement(TAG_N);
    n.appendChild(root.ownerDocument().createTextNode(QString::number(this->n)));
    root.appendChild(n);

    QString tag_header;
    QDomElement force;
    QString data;
    QDomElement element;

    QDomElement data_zero = root.ownerDocument().createElement(TAG_DATA_ZERO);
    root.appendChild(data_zero);

    name = root.ownerDocument().createElement(TAG_ZERO_NAME);
    name.appendChild(root.ownerDocument().createTextNode(this->zero_name));
    data_zero.appendChild(name);

    element = root.ownerDocument().createElement(TAG_ITEM);
    data_zero.appendChild(element);
    foreach (VariableModel *var, variables) {
        if (var->meta->has_zero()){
            for (int k=0 ; k<var->meta->get_num(); k++){
                tag_header = var->meta->get_name(k).simplified();
                tag_header.replace(" ","_");
                force = root.ownerDocument().createElement(tag_header);
                data = QString::number( var->get_zero().value(k) ,'g',10);
                force.appendChild( root.ownerDocument().createTextNode(data));
                element.appendChild(force);
            }
        }
    }

    QDomElement data_element = root.ownerDocument().createElement(TAG_DATA);
    root.appendChild(data_element);
    for (int row=0; row < rowCount(); row++){
        element = root.ownerDocument().createElement(TAG_ITEM);
        data_element.appendChild(element);
        for (int column =0; column < columnCount(); column++){
            tag_header = this->headerData(column,Qt::Horizontal).toString().simplified();
            tag_header.replace(" ","_");
            force = root.ownerDocument().createElement(tag_header);
            data = this->data(this->index(row,column)).toString();
            force.appendChild( root.ownerDocument().createTextNode(data));
            element.appendChild(force);
        }
    }
}


void MeasurementsModel::load_xml(QDomElement root){
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
        if (element.tagName() == TAG_AVERAGE_NUMBER){
            this->average_number = element.text().toInt();
            continue;
        }
        if (element.tagName() == TAG_CONTROL_TYPE){
            this->control = element.text();
        }
        if (element.tagName() == TAG_END){
            this->end = element.text().toDouble();
            continue;
        }
        if (element.tagName() == TAG_STEP){
            this->step = element.text().toDouble();
            continue;
        }
        if (element.tagName() == TAG_SETTLING_TIME){
            this->settling_time = element.text().toInt();
            continue;
        }
        if (element.tagName() == TAG_N){
            this->n = element.text().toInt();
            continue;
        }

        if (element.tagName() == TAG_START_VALUES){
            QDomNodeList items = element.childNodes();
            QDomElement item;

            for (int k = 0; k < items.count(); k++){
                item = items.at(k).toElement();

                if (item.tagName() == TAG_ITEM){
                    QDomNodeList vars = item.childNodes();
                    QDomElement var;
                    for (int n = 0; n < vars.count(); n++ ){
                        var = vars.at(n).toElement();
                        start_hash[var.nodeName()] = var.text().toDouble();
                    }
                }
            }
            continue;
        }

        if (element.tagName() == TAG_DATA_ZERO){
            QDomNodeList items = element.childNodes();
            QDomElement item;

            for (int row = 0; row < items.count(); row++){
                item = items.at(row).toElement();

                if (item.tagName() == TAG_ZERO_NAME){
                    this->zero_name = item.text();
                }
                if (item.tagName() == TAG_ITEM){
                    QDomNodeList forces = item.childNodes();
                    QDomElement force;
                    QString tag;
                    foreach (VariableModel *var, variables) {
                        if(var->meta->has_zero()){
                            QVector<double> vector;
                            for (int k=0; k< var->meta->get_num(); k++){
                                for (int column = 0; column< forces.count(); column++ ){
                                    force = forces.at(column).toElement();
                                    tag = force.tagName();
                                    if (tag == var->meta->get_name(k)){
                                        vector.append(force.text().toDouble());
                                        continue;
                                    }
                                }
                            }
                            var->set_zero(vector);
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
                    this->setData(index(row,column),force.text());
                }
            }
            continue;
        }
    }
}

bool MeasurementsModel::setData ( const QModelIndex & index, const QVariant & value, int role){
    if (!index.isValid()){
        return false;
    }

    if (role == Qt::EditRole) {
        int row = index.row();
        if (rowCount() < row){
            return false;
        }

        int upper = 0;
        int lower = 0;
        int column = index.column();
        foreach (VariableModel *var, variables) {
            lower = upper;
            upper += var->meta->get_num();
            if ( column < upper){
                var->set_value(column - lower,row,value.toDouble());
                return true;
            }
        }
    }
    return true;
}

bool MeasurementsModel::insertRows ( int row, int count, const QModelIndex & parent ){
    Q_UNUSED(parent)

    if (row < 0 ){
        return false;
    }

    foreach (VariableModel *var, variables) {
        for (int k=0; k< var->meta->get_num(); k++){
            var->insert_value(k, row, count, 0);
        }
    }

    return true;
}
bool MeasurementsModel::insertRow ( int row,  const QModelIndex & parent ){
    return insertRows(row,1,parent);
}
