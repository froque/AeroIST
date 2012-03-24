#ifndef REFERENCEPREFERENCES_H
#define REFERENCEPREFERENCES_H

#include <QDialog>
#include <QSettings>
#include "referencemodel.h"
#include "referencelist.h"
#include <QDoubleSpinBox>

namespace Ui {
    class ReferencePreferences;
}

class ReferencePreferences : public QDialog
{
    Q_OBJECT

public:
    ReferencePreferences(ReferenceModel *measurement,QWidget *parent = 0);
    ~ReferencePreferences();

private slots:
    void accept();

private:
    Ui::ReferencePreferences *ui;
    ReferenceModel *measurement;
    QList<QDoubleSpinBox*> list_start;
};

#endif // REFERENCEPREFERENCES_H
