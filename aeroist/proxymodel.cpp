#include "proxymodel.h"

ProxyModel::ProxyModel(){
    show_raw=false;
}

QVariant ProxyModel::data(const QModelIndex & proxyIndex, int role ) const{
    if (role != Qt::DisplayRole){
        return QIdentityProxyModel::data(proxyIndex, role);
    }
    // Qt::DisplayRole - show normal values
    // Qt::UserRole    - show raw values
    QVariant var;
    if (show_raw==false){
        var = sourceModel()->data(proxyIndex,Qt::DisplayRole);
    } else {
        var = sourceModel()->data(proxyIndex,Qt::UserRole);
    }
    return var;
}

QVariant ProxyModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole){
        return sourceModel()->headerData(section,orientation,role);
    }

    // Qt::DisplayRole - don't show units
    // Qt::UserRole    - dont't translate
    // Qt::UserRole +1 - translate and show units
    // Qt::UserRole +2 - translate and show raw units
    QVariant var;
    if (show_raw==false){
        var = sourceModel()->headerData(section,orientation,Qt::UserRole+1);
    } else {
        var = sourceModel()->headerData(section,orientation,Qt::UserRole+2);
    }
    return var;
}
