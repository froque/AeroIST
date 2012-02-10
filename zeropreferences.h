#ifndef ZEROPREFERENCES_H
#define ZEROPREFERENCES_H

#include <QDialog>
#include <QSettings>
#include "zeromodel.h"
#include "zerolist.h"
#include <QDoubleSpinBox>

namespace Ui {
    class ZeroPreferences;
}

class ZeroPreferences : public QDialog
{
    Q_OBJECT

public:
    ZeroPreferences(ZeroModel *measurement,QWidget *parent = 0);
    ~ZeroPreferences();

private slots:
    void accept();

private:
    Ui::ZeroPreferences *ui;
    ZeroModel *measurement;
    QList<QDoubleSpinBox*> list_start;
};

#endif // ZEROPREFERENCES_H
