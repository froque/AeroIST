#include "zerolist.h"

ZeroList::ZeroList(QObject *parent) :
    QAbstractListModel(parent)
{
    Q_UNUSED(parent);
}

int ZeroList::rowCount ( const QModelIndex & parent ) const {
    Q_UNUSED(parent);
    return list.size();
}

QVariant ZeroList::data ( const QModelIndex & index, int role ) const{
    Q_UNUSED(index);

    if (role == Qt::DisplayRole){
        return list.value(index.row())->name;
    }
    if(role == Qt::UserRole){
        return list.value(index.row())->matrix;
    }
    return QVariant();
}

void ZeroList::newMeasure(ZeroModel * measure){
    beginInsertRows(QModelIndex(), list.size(), list.size());
    list.append(measure);
    endInsertRows();
}

void ZeroList::deleteMeasure(QModelIndex index){
    beginRemoveRows(QModelIndex(),0,list.size());
    if (index.isValid()){
        delete list.takeAt(index.row());
    }
    endRemoveRows();
}

ZeroModel * ZeroList::at(int position){
    return list.value(position);
}

ZeroModel * ZeroList::at(QModelIndex index){
    if (index.isValid()){
        return list.value(index.row());
    }
    return NULL;
}

void ZeroList::clear(void){
    while (list.size()>0){
         deleteMeasure( index(0,0,QModelIndex()) );
    }
}

void ZeroList::save_xml(QDomElement root){
    ZeroModel *zero;
    for (int k=0; k<rowCount(); k++){
        zero = list.at(k);
        QDomElement zero_element = root.ownerDocument().createElement(TAG_ZERO);
        root.appendChild(zero_element);

        zero->save_xml(zero_element);
    }
}
void ZeroList::load_xml(QDomElement root){
    QDomNodeList nodeslist = root.elementsByTagName(TAG_ZERO);
    QDomNode node;
    QDomElement element;
    ZeroModel *zero;
    for (int k=0; k<nodeslist.count(); k++){
        node = nodeslist.at(k);
        element = node.toElement();
        zero =  new ZeroModel(element);
        newMeasure(zero);
    }
}
