#include "referencelist.h"

ReferenceList::ReferenceList(QObject *parent) :
    QAbstractListModel(parent)
{
    Q_UNUSED(parent);
}

ReferenceList::~ReferenceList(){
    qDeleteAll(list);
}

int ReferenceList::rowCount ( const QModelIndex & parent ) const {
    Q_UNUSED(parent);
    return list.size();
}

QVariant ReferenceList::data ( const QModelIndex & index, int role ) const{
    Q_UNUSED(index);

    if (role == Qt::DisplayRole){
        return list.value(index.row())->name;
    }
    return QVariant();
}

void ReferenceList::newMeasure(ReferenceModel * measure){
    beginInsertRows(QModelIndex(), list.size(), list.size());
    list.append(measure);
    endInsertRows();
}

void ReferenceList::deleteMeasure(QModelIndex index){
    beginRemoveRows(QModelIndex(),index.row(),index.row());
    if (index.isValid()){
        delete list.takeAt(index.row());
    }
    endRemoveRows();
}

ReferenceModel * ReferenceList::at(int position){
    return list.value(position);
}

ReferenceModel * ReferenceList::at(QModelIndex index){
    if (index.isValid()){
        return list.value(index.row());
    }
    return NULL;
}

void ReferenceList::clear(void){
    while (list.size()>0){
         deleteMeasure( index(0,0,QModelIndex()) );
    }
}

void ReferenceList::save_xml(QDomElement root){
    ReferenceModel *ref;
    for (int k=0; k<rowCount(); k++){
        ref = list.at(k);
        QDomElement ref_element = root.ownerDocument().createElement(TAG_REFERENCE);
        root.appendChild(ref_element);

        ref->save_xml(ref_element);
    }
}
void ReferenceList::load_xml(QDomElement root){
    QDomNodeList nodeslist = root.elementsByTagName(TAG_REFERENCE);
    QDomNode node;
    QDomElement element;
    ReferenceModel *ref;
    for (int k=0; k<nodeslist.count(); k++){
        node = nodeslist.at(k);
        element = node.toElement();
        ref =  new ReferenceModel(element);
        newMeasure(ref);
    }
}
