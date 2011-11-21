#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QSettings>

#include <QDialog>

namespace Ui {
    class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QSettings *settings,QWidget *parent = 0);
    ~Preferences();

private slots:
    void on_buttonBox_accepted();


    void on_buttonBox_rejected();

private:
    Ui::Preferences *ui;
    QSettings *settings;
};

#endif // PREFERENCES_H
