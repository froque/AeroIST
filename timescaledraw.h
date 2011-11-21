#ifndef TIMESCALEDRAW_H
#define TIMESCALEDRAW_H

#include <QTime>
#include <qwt/qwt_scale_draw.h>


class TimeScaleDraw : public QwtScaleDraw
{

public:
    TimeScaleDraw(const QTime &base):
        baseTime(base)
    {
    }
    virtual QwtText label(double v) const
    {
        QTime upTime = baseTime.addSecs((int)v);
        return upTime.toString();
    }
private:
    QTime baseTime;
};

#endif // TIMESCALEDRAW_H
