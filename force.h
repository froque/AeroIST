#ifndef FORCE_H
#define FORCE_H

#include "common.h"
#include <QString>

// this belongs here
#define READBUFFER 12     /* L0 - short message */
#define NUMCHANNELS 6
#define ANZ_QUA_ELE 21
// this maybe in a config file:
//#define NOMINALLOAD 100.0 /* kg    */
#define GRAVITY 9.81      /* m/s^2 */
#define SENSITIVITY 2.0   /* mV/V  */
#define VCC 12            /* V     */
#define MAX_ITER 10
#define TOLERANCE 1e-5

#pragma pack(1)
struct matrix		      /* abgespeicherte Kalibriermatrix */
{
        double coef_lin[NUMCHANNELS][NUMCHANNELS];
        double coef_qua [ANZ_QUA_ELE][NUMCHANNELS];
} ;//balance;
#pragma pack()


class Force {
public:
    Force(matrix_t matrix, int dvm_time);
    ~Force();
    void read(void);
    double dvm_si[NUMCHANNELS];

private:
    int dvm_time;
    matrix_t matrix;
    QString filename;

    double forces[NUMCHANNELS];
    double dvm[NUMCHANNELS];

    double nominal_load[NUMCHANNELS];
    struct matrix coe;
    struct matrix mat;

    double ascii2newton (char *buf);
    void read_dvm(void);
    void convert_dvm();

    void calc_jacobi(double force[],double jm[NUMCHANNELS][NUMCHANNELS]);
    void calc_function(double F[NUMCHANNELS]);
    bool check_tolerance(double F[NUMCHANNELS]);
    void newton_method();

    int g_id;
    int k;
};

#endif // FORCE_H
