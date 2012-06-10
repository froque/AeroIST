#include "measurelist.h"


MeasureList::MeasureList (QObject * parent ) : QAbstractListModel(parent){
    Q_UNUSED(parent);
}

MeasureList::~MeasureList(){
    qDeleteAll(list);
}

int MeasureList::rowCount ( const QModelIndex & parent  ) const{
    Q_UNUSED(parent);
    return list.size();
}

QVariant MeasureList::data ( const QModelIndex & index, int role ) const{
    if (role == Qt::DisplayRole){
        return list.value(index.row())->name;
    }
    return QVariant();
}

void MeasureList::newMeasure(MeasurementsModel * measure){
    beginInsertRows(QModelIndex(), list.size(), list.size());
    list.append(measure);
    endInsertRows();
}

void MeasureList::deleteMeasure(QModelIndex index){
    beginRemoveRows(QModelIndex(),index.row(),index.row());
    if (index.isValid()){
        delete list.takeAt(index.row());
    }
    endRemoveRows();
}

MeasurementsModel * MeasureList::at(int position){
    return list.value(position);
}

MeasurementsModel * MeasureList::at(QModelIndex index){
    if (index.isValid()){
        return list.value(index.row());
    }
    return NULL;
}

void MeasureList::clear(void){
    while (list.size()>0){
         deleteMeasure( index(0,0,QModelIndex()) );
    }
}

void MeasureList::save_xml(QDomElement root){
    MeasurementsModel *measurement;
    for (int k=0; k<rowCount(); k++){
        measurement = list.at(k);
        QDomElement measurement_element = root.ownerDocument().createElement(TAG_MEASURE);
        root.appendChild(measurement_element);

        measurement->save_xml(measurement_element);
    }
}

void MeasureList::load_xml(QDomElement root){
    QDomNodeList nodeslist = root.elementsByTagName(TAG_MEASURE);
    QDomNode node;
    QDomElement element;
    MeasurementsModel *measure;
    for (int k=0; k<nodeslist.count(); k++){
        node = nodeslist.at(k);
        element = node.toElement();
        measure =  new MeasurementsModel(element);
        newMeasure(measure);
    }
}
