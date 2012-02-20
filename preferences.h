#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QSettings>

#include <QDialog>
#include "variable.h"

namespace Ui {
    class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();

private slots:
    void on_buttonBox_accepted();
    void on_toolButton_clicked();
//    void on_toolButton_2_clicked();
//    void on_toolButton_3_clicked();
//    void on_toolButton_4_clicked();
    void on_toolButton_5_clicked();

private:
    Ui::Preferences *ui;
    QList<VariablePreferences*> variables;
};

#endif // PREFERENCES_H
