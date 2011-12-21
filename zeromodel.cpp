#include "zeromodel.h"

ZeroModel::ZeroModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    name="";
    dvm_time=0;
    matrix=MIDDLE;
    average_number=0;

}

ZeroModel::ZeroModel(QDomElement root,QObject *parent) :
    QAbstractTableModel(parent)
{
    load_xml(root);
}

int ZeroModel::rowCount(const QModelIndex &parent) const{
    Q_UNUSED(parent);
    return force[0].size();
}

int ZeroModel::columnCount(const QModelIndex &parent) const{
    Q_UNUSED(parent);
    return NVARS_ZERO;
}

QVariant ZeroModel::data(const QModelIndex &index, int role) const{
    if (!index.isValid()){
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        int row = index.row();
        switch (index.column()) {
            case 0:
                return force[0].value(row);
            case 1:
                return force[1].value(row);
            case 2:
                return force[2].value(row);
            case 3:
                return force[3].value(row);
            case 4:
                return force[4].value(row);
            case 5:
                return force[5].value(row);
        }
    }
    return QVariant();
}

QVariant ZeroModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Fx");
            case 1:
                return tr("Fy");
            case 2:
                return tr("Fz");
            case 3:
                return tr("Mx");
            case 4:
                return tr("My");
            case 5:
                return tr("Mz");
            default:
                return QVariant();
        }
    }
    return QVariant();
}

void ZeroModel::GetMeasure(measure m){
    beginInsertRows(QModelIndex(), force[0].size(), force[0].size());
    for (int k=0;k<6;k++ ){
        force[k].append(m.force[k]);
    }
    endInsertRows();
}

void ZeroModel::save_xml(QDomElement root){
    QDomElement name = root.ownerDocument().createElement(TAG_NAME);
    name.appendChild(root.ownerDocument().createTextNode(this->name));
    root.appendChild(name);

    QDomElement description = root.ownerDocument().createElement(TAG_DESCRIPTION);
    description.appendChild(root.ownerDocument().createTextNode(this->description));
    root.appendChild(description);

    QDomElement dvm_time = root.ownerDocument().createElement(TAG_DVM_TIME);
    dvm_time.appendChild(root.ownerDocument().createTextNode(QString::number(this->dvm_time)));
    root.appendChild(dvm_time);

    QDomElement matrix = root.ownerDocument().createElement(TAG_MATRIX);
    matrix.appendChild(root.ownerDocument().createTextNode(QString::number(this->matrix)));
    root.appendChild(matrix);

    QDomElement average_number = root.ownerDocument().createElement(TAG_AVERAGE_NUMBER);
    average_number.appendChild(root.ownerDocument().createTextNode(QString::number(this->average_number)));
    root.appendChild(average_number);

    QDomElement set_alpha = root.ownerDocument().createElement(TAG_SET_ALPHA);
    set_alpha.appendChild(root.ownerDocument().createTextNode(QString::number(this->set_alpha)));
    root.appendChild(set_alpha);

    QDomElement set_beta = root.ownerDocument().createElement(TAG_SET_BETA);
    set_beta.appendChild(root.ownerDocument().createTextNode(QString::number(this->set_beta)));
    root.appendChild(set_beta);

    QDomElement set_motor = root.ownerDocument().createElement(TAG_SET_MOTOR);
    set_motor.appendChild(root.ownerDocument().createTextNode(QString::number(this->set_motor)));
    root.appendChild(set_motor);

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
            tag_header = this->headerData(column,Qt::Horizontal).toString().simplified();
            tag_header.replace(" ","_");
            force = root.ownerDocument().createElement(tag_header);
            data = this->data(this->index(row,column)).toString();
            force.appendChild( root.ownerDocument().createTextNode(data));
            element.appendChild(force);
        }
    }
}

void ZeroModel::load_xml(QDomElement root){

    QDomNodeList nodelist = root.childNodes();
    QDomNode node;
    QDomElement element;
    for (int k=0; k< nodelist.count();k++){
        node = nodelist.at(k);
        element = node.toElement();
        if (element.tagName() == TAG_NAME){
            this->name = element.text();
        }
        if (element.tagName() == TAG_DESCRIPTION){
            this->description = element.text();
        }
        if (element.tagName() == TAG_DVM_TIME){
            this->dvm_time = element.text().toInt();
        }
        if (element.tagName() == TAG_AVERAGE_NUMBER){
            this->average_number = element.text().toInt();
        }
        if (element.tagName() == TAG_SET_ALPHA){
            this->set_alpha = element.text().toDouble();
        }
        if (element.tagName() == TAG_SET_BETA){
            this->set_beta = element.text().toDouble();
        }
        if (element.tagName() == TAG_SET_MOTOR){
            this->set_motor = element.text().toDouble();
        }
        if (element.tagName() == TAG_MATRIX){
            int m = element.text().toInt();
            switch (m){
            case FLOOR: this->matrix = FLOOR; break;
            case MIDDLE: this->matrix = MIDDLE; break;
            }
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
                    this->setData(this->index(row,column),force.text());
                }
            }
        }
    }
}


bool ZeroModel::setData ( const QModelIndex & index, const QVariant & value, int role){
    if (!index.isValid()){
        return false;
    }

    if (role == Qt::EditRole) {
        int row = index.row();
        if (force[0].size() < row ){
            return false;
        }
        switch (index.column()) {
        case 0:
            force[0].replace(row,value.toDouble());
        case 1:
            force[1].replace(row,value.toDouble());
        case 2:
            force[2].replace(row,value.toDouble());
        case 3:
            force[3].replace(row,value.toDouble());
        case 4:
            force[4].replace(row,value.toDouble());
        case 5:
            force[5].replace(row,value.toDouble());
        }
    }
    return true;
}

bool ZeroModel::insertRows ( int row, int count, const QModelIndex & parent ){
    Q_UNUSED(parent)
    if (row <0 || row > force[0].size()){
        return false;
    }
    for (int k=0; k< NVARS_ZERO; k++){
        force[k].insert(row,count,-1);
    }
    return true;
}
bool ZeroModel::insertRow ( int row,  const QModelIndex & parent ){
    return insertRows(row,1,parent);
}
