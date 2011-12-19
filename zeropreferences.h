#ifndef ZEROPREFERENCES_H
#define ZEROPREFERENCES_H

#include <QDialog>
#include <QSettings>
#include "zeromodel.h"
#include "zerolist.h"

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
//    void on_buttonBox_accepted();
//    void test_input();
    void accept();

private:
    Ui::ZeroPreferences *ui;
    ZeroModel *measurement;
};

#endif // ZEROPREFERENCES_H
