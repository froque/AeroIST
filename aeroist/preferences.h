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
    void accept(void);
    void on_toolButton_clicked();

private:
    Ui::Preferences *ui;
    QList<VariablePreferences*> variables;
};

#endif // PREFERENCES_H
