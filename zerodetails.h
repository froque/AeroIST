#ifndef ZERODETAILS_H
#define ZERODETAILS_H

#include <QDialog>
#include "zeromodel.h"
#include "common.h"

#ifdef DEBUG
#include <QDebug>
#endif //DEBUG

namespace Ui {
    class ZeroDetails;
}

class ZeroDetails : public QDialog
{
    Q_OBJECT

public:
    explicit ZeroDetails(ZeroModel *measurement, QWidget *parent = 0);
    ~ZeroDetails();

private:
    Ui::ZeroDetails *ui;
};

#endif // ZERODETAILS_H








