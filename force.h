#ifndef FORCE_H
#define FORCE_H

#include "common.h"
#include <QString>

#define READBUFFER 12     /* L0 - short message */
#define NUMCHANNELS 6
#define ANZ_QUA_ELE 21


#pragma pack(1)
struct matrix		      /* abgespeicherte Kalibriermatrix */
{
        double coef_lin[NUMCHANNELS][NUMCHANNELS];
        double coef_qua [ANZ_QUA_ELE][NUMCHANNELS];
} ;
#pragma pack()


class Force {
public:
    Force(matrix_t matrix, int dvm_time,double zero[NUMCHANNELS]);
    Force(matrix_t matrix, int dvm_time);
    void initialize();
    ~Force();
    void read(void);
    double forces[NUMCHANNELS];
    bool isReady(void);

private:
    int dvm_time;
    matrix_t matrix;
    QString filename;

    double dvm_si[NUMCHANNELS];
    double zero[NUMCHANNELS];
    double dvm_si_zero[NUMCHANNELS];
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
