#include "proxymodel.h"

ProxyModel::ProxyModel(){
    show_raw=false;
}

QVariant ProxyModel::data(const QModelIndex & proxyIndex, int role ) const{
    if (role != Qt::DisplayRole){
        return QIdentityProxyModel::data(proxyIndex, role);
    }
    QVariant var;
    if (show_raw==false){
        var = sourceModel()->data(proxyIndex,Qt::DisplayRole);
    } else {
        var = sourceModel()->data(proxyIndex,Qt::UserRole);
    }
    return var;
}
