#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QIdentityProxyModel>

class ProxyModel : public QIdentityProxyModel
{
public:
    ProxyModel();
    QVariant data(const QModelIndex & proxyIndex, int role = Qt::DisplayRole ) const;
    bool show_raw;
};

#endif // PROXYMODEL_H
