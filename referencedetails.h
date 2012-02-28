#ifndef REFERENCEDETAILS_H
#define REFERENCEDETAILS_H

#include <QDialog>
#include "referencemodel.h"
#include "common.h"

namespace Ui {
    class ReferenceDetails;
}

class ReferenceDetails : public QDialog
{
    Q_OBJECT

public:
    explicit ReferenceDetails(ReferenceModel *measurement, QWidget *parent = 0);
    ~ReferenceDetails();

private:
    Ui::ReferenceDetails *ui;
};

#endif // REFERENCEDETAILS_H








