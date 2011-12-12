#include "measurelist.h"


MeasureList::MeasureList (QObject * parent ) : QAbstractListModel(parent)
{
    Q_UNUSED(parent);
    freeId = 0 ;
}

int MeasureList::rowCount ( const QModelIndex & parent  ) const
{
    Q_UNUSED(parent);
    return list.size();
}

QVariant MeasureList::data ( const QModelIndex & index, int role ) const{
    if (role == Qt::DisplayRole){
        return list.value(index.row())->name;
    }
    if (role == Qt::UserRole){
        return list.value(index.row())->matrix;
    }
    return QVariant();
}

void MeasureList::newMeasure(MeasurementsModel * measure)
{
    beginInsertRows(QModelIndex(), list.size(), list.size());
    list.append(measure);
    endInsertRows();
}

void MeasureList::deleteMeasure(QModelIndex index)
{
//    MeasurementsModel *measurement;
    beginRemoveRows(QModelIndex(),0,list.size());
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
//Qt::DropActions MeasureList::supportedDropActions( void ) const
//{
//    return Qt::MoveAction|Qt::CopyAction;
//}

//Qt::ItemFlags MeasureList::flags(const QModelIndex &index) const
//{
//    if (!index.isValid())
//        return 0;

//    return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled;
//}

//bool MeasureList::insertRow ( int row, const QModelIndex & parent ){
//    insertRows(row,1,parent);
//}

//bool MeasureList::insertRows ( int position, int count, const QModelIndex & parent ){
//    qDebug() << "insertrows";
//    beginInsertRows(parent,position,position+count-1);
//    qDebug() << "insertrows";
//    MeasurementsModel *temp=NULL;
//    for (int row=0; row < count; row++) {
//        list.insert(position+row,temp);
//    }
//    endInsertRows();
//}
//bool MeasureList::removeRow(int position, const QModelIndex &index)
//{
//    removeRows(position,1,index);
//}
//bool MeasureList::removeRows(int position, int rows, const QModelIndex &index)
//{
//    qDebug() << "removerows";
//    Q_UNUSED(index);
//    beginRemoveRows(index, position, position+rows-1);
//    for (int row=0; row < rows; ++row) {
//        list.removeAt(row);
//    }
//    endRemoveRows();
//    return true;
//}

//bool MeasureList::setData ( const QModelIndex & index, const QVariant & value, int role ){
//    qDebug() << "setdata";

//    if (index.isValid() && role == Qt::EditRole) {
//        MeasurementsModel *measurement = (MeasurementsModel *) value.value<void *>();
//        list.replace(index.row(),measurement);
//        emit(dataChanged(index, index));
//        return true;
//    }
//     return false;
//}

//bool MeasureList::setItemData ( const QModelIndex & index, const QMap<int, QVariant> & roles ){
//    qDebug() << "setitemdata";
//}

void MeasureList::save(QString fileName){
    MeasurementsModel *measurement;
    QFile file(fileName);

    if (file.open(QFile::WriteOnly|QFile::Text)){
        QTextStream out(&file);

        for (int k=0; k < list.size() ;k++){
            measurement = at(k);
            measurement->save(&out);
        }
        file.close();
    }
}

void MeasureList::clear(void){
    while (list.size()>0){
         deleteMeasure( index(0,0,QModelIndex()) );
    }
}

/*
void MeasureList::load(QString fileName){
    QFile file(fileName);
    if (file.open(QFile::ReadOnly|QFile::Text)){
        QTextStream in(&file);
        MeasurementsModel *measurement;
        while(in.atEnd() == false){

            measurement = new MeasurementsModel();
            measurement->load(&in);
            newMeasure(measurement);
        }
    }
    file.close();
}*/

bool MeasureList::zeroUsed(ZeroModel *zero){
    MeasurementsModel *measurement;
    foreach(measurement,list){
        if (zero == measurement->zero){
            return true;
        }
    }
    return false;
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

int MeasureList::getFreeId(){
    return freeId++;
}

void MeasureList::load_xml(QDomElement root){
    QDomNodeList nodeslist = root.elementsByTagName(TAG_MEASURE);
    QDomNode node;
    QDomElement element;
    MeasurementsModel *measure;
    for (int k=0; k<nodeslist.count(); k++){
        node = nodeslist.at(k);
        element = node.toElement();
        qDebug() << k << "measurelist loop";
        measure =  new MeasurementsModel(element);
        newMeasure(measure);
    }
}
