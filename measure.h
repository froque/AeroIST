#ifndef MEASURE_H
#define MEASURE_H

#include <QTime>

struct measure{
//    QTime tempo;
    double tempo;
    double force[6];
    double alpha;
    double beta;
    double motor;
    double temp;
    double wind;
};

#endif // MEASURE_H
