#include <QtGlobal>
#include <QStringList>
#include "measurementsmodel.h"


#ifdef DEBUG
#include "QDebug"
#endif // DEBUG

MeasurementsModel::MeasurementsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    name="";
    dvm_time=0;
    matrix=MIDDLE;
    average_number=0;
    settling_time=0;
    start=0;
    end=0;
    step=0;
    control_type = NONE;
    n = 0;
}
MeasurementsModel::MeasurementsModel(QDomElement root, QObject *parent):
    QAbstractTableModel(parent)
{
    load_xml(root);
}

void MeasurementsModel::save(QTextStream *out ){
    *out << name << endl;
    *out << dvm_time << endl;
    *out << matrix << endl;
    int size = rowCount(QModelIndex());
    *out << size << endl;

    save_csv(out,false);

}

void MeasurementsModel::save_csv(QTextStream *out,bool header){
    int rows = rowCount(QModelIndex());
    int columns = columnCount(QModelIndex());

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

void MeasurementsModel::GetMeasure(measure m){
    beginInsertRows(QModelIndex(), tempo.size(), tempo.size());

    for (int k=0;k<6;k++ ){
        force[k].append(m.force[k]);
    }
    alpha.append( m.alpha);
    beta.append( m.beta );
    motor.append( m.motor );
    temp.append( m.temp );
    tempo.append(m.tempo);
    wind.append(m.wind);
    endInsertRows();
}

int MeasurementsModel::columnCount(const QModelIndex &parent)  const{
    Q_UNUSED(parent);
    return NVARS;
}

int MeasurementsModel::rowCount(const QModelIndex &parent ) const{
    Q_UNUSED(parent);
    return force[0].size();
}

QVariant MeasurementsModel::data(const QModelIndex &index, int role) const{
    if (!index.isValid()){
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        int row = index.row();
        switch (index.column()) {
        case 0:
            return tempo.value(row);
        case 1:
            return force[0].value(row);
        case 2:
            return force[1].value(row);
        case 3:
            return force[2].value(row);
        case 4:
            return force[3].value(row);
        case 5:
            return force[4].value(row);
        case 6:
            return force[5].value(row);
        case 7:
            return alpha.value(row);
        case 8:
            return beta.value(row);
        case 9:
            return motor.value(row);
        case 10:
            return temp.value(row);
        case 11:
            return wind.value(row);
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
        switch (section) {
        case 0:
            return tr("Time");
        case 1:
            return tr("Fx");
        case 2:
            return tr("Fy");
        case 3:
            return tr("Fz");
        case 4:
            return tr("Mx");
        case 5:
            return tr("My");
        case 6:
            return tr("Mz");
        case 7:
            return tr("Alpha");
        case 8:
            return tr("Beta");
        case 9:
            return tr("Motor");
        case 10:
            return tr("Temperature");
        case 11:
            return tr("Wind");
        default:
            return QVariant();
        }
    }
    return QVariant();
}


QVector<double>  MeasurementsModel::vector_data(int index){
    switch (index) {
    case 0:
        return tempo;
    case 1:
        return force[0];
    case 2:
        return force[1];
    case 3:
        return force[2];
    case 4:
        return force[3];
    case 5:
        return force[4];
    case 6:
        return force[5];
    case 7:
        return alpha;
    case 8:
        return beta;
    case 9:
        return motor;
    case 10:
        return temp;
    case 11:
        return wind;
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

    QDomElement dvm_time = root.ownerDocument().createElement(TAG_DVM_TIME);
    dvm_time.appendChild(root.ownerDocument().createTextNode(QString::number(this->dvm_time)));
    root.appendChild(dvm_time);

    QDomElement matrix = root.ownerDocument().createElement(TAG_MATRIX);
    matrix.appendChild(root.ownerDocument().createTextNode(QString::number(this->matrix)));
    root.appendChild(matrix);

    QDomElement average_number = root.ownerDocument().createElement(TAG_AVERAGE_NUMBER);
    average_number.appendChild(root.ownerDocument().createTextNode(QString::number(this->average_number)));
    root.appendChild(average_number);

    QDomElement settling_time = root.ownerDocument().createElement(TAG_SETTLING_TIME);
    settling_time.appendChild(root.ownerDocument().createTextNode(QString::number(this->settling_time)));
    root.appendChild(settling_time);

    QDomElement start = root.ownerDocument().createElement(TAG_START);
    start.appendChild(root.ownerDocument().createTextNode(QString::number(this->start)));
    root.appendChild(start);

    QDomElement end = root.ownerDocument().createElement(TAG_END);
    end.appendChild(root.ownerDocument().createTextNode(QString::number(this->end)));
    root.appendChild(end);

    QDomElement step = root.ownerDocument().createElement(TAG_STEP);
    step.appendChild(root.ownerDocument().createTextNode(QString::number(this->step)));
    root.appendChild(step);

    QDomElement set_alpha = root.ownerDocument().createElement(TAG_SET_ALPHA);
    set_alpha.appendChild(root.ownerDocument().createTextNode(QString::number(this->set_alpha)));
    root.appendChild(set_alpha);

    QDomElement set_beta = root.ownerDocument().createElement(TAG_SET_BETA);
    set_beta.appendChild(root.ownerDocument().createTextNode(QString::number(this->set_beta)));
    root.appendChild(set_beta);

    QDomElement set_motor = root.ownerDocument().createElement(TAG_SET_MOTOR);
    set_motor.appendChild(root.ownerDocument().createTextNode(QString::number(this->set_motor)));
    root.appendChild(set_motor);

    QDomElement control_type = root.ownerDocument().createElement(TAG_CONTROL_TYPE);
    control_type.appendChild(root.ownerDocument().createTextNode(QString::number(this->control_type)));
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
    for (int column =0; column < NFORCES; column++){
        tag_header = this->headerData(column+1,Qt::Horizontal).toString().simplified();
        tag_header.replace(" ","_");
        force = root.ownerDocument().createElement(tag_header);
        data = QString::number(zero[column],'g',10);
        force.appendChild( root.ownerDocument().createTextNode(data));
        element.appendChild(force);
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
        if (element.tagName() == TAG_CONTROL_TYPE){
            int m = element.text().toInt();
            switch (m){
            case NONE: this->control_type = NONE; break;
            case ALPHA: this->control_type = ALPHA; break;
            case BETA: this->control_type = BETA; break;
            case MOTOR: this->control_type = MOTOR; break;
            }
        }
        if (element.tagName() == TAG_START){
            this->start = element.text().toDouble();
        }
        if (element.tagName() == TAG_END){
            this->end = element.text().toDouble();
        }
        if (element.tagName() == TAG_STEP){
            this->step = element.text().toDouble();
        }
        if (element.tagName() == TAG_SETTLING_TIME){
            this->settling_time = element.text().toInt();
        }
        if (element.tagName() == TAG_N){
            this->n = element.text().toInt();
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
                    for (int column = 0; column< forces.count(); column++ ){
                        force = forces.at(column).toElement();
                        zero[column] = force.text().toDouble();
                    }
                }
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

bool MeasurementsModel::setData ( const QModelIndex & index, const QVariant & value, int role){
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
            tempo.replace(row,value.toDouble());
        case 1:
            force[0].replace(row,value.toDouble());
        case 2:
            force[1].replace(row,value.toDouble());
        case 3:
            force[2].replace(row,value.toDouble());
        case 4:
            force[3].replace(row,value.toDouble());
        case 5:
            force[4].replace(row,value.toDouble());
        case 6:
            force[5].replace(row,value.toDouble());
        case 7:
            alpha.replace(row,value.toDouble());
        case 8:
            beta.replace(row,value.toDouble());
        case 9:
            motor.replace(row,value.toDouble());
        case 10:
            temp.replace(row,value.toDouble());
        case 11:
            wind.replace(row,value.toDouble());
        }
    }
    return true;
}

bool MeasurementsModel::insertRows ( int row, int count, const QModelIndex & parent ){
    Q_UNUSED(parent)
    if (row <0 || row > force[0].size()){
        return false;
    }
    tempo.insert(row,count,0);
    for (int k=0; k< NVARS_ZERO; k++){
        force[k].insert(row,count,0);
    }
    alpha.insert(row,count,0);
    beta.insert(row,count,0);
    motor.insert(row,count,0);
    temp.insert(row,count,0);
    wind.insert(row,count,0);
    return true;
}
bool MeasurementsModel::insertRow ( int row,  const QModelIndex & parent ){
    return insertRows(row,1,parent);
}
