#ifndef FORCE_H
#define FORCE_H

#include "common.h"
#include <QString>
#include "variable.h"

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


class Force :public Variable{
public:
    Force(matrix_t matrix, int dvm_time,double zero[NUMCHANNELS]);
    Force(matrix_t matrix, int dvm_time);
    ~Force();
    void read(void);

    bool is_controlable() {return false;};
    int get_num() {return 6;};
    double get_value(int n) {return forces[n];};
    void set_value(int n ,double value) {Q_UNUSED(n);Q_UNUSED(value)};
    QString get_name(int n) ;
    bool isReady(void) {return true;};
    bool has_set_final() {return false;}
    void set_final() {}
private:
    int dvm_time;
    matrix_t matrix;
    QString filename;

    double forces[NUMCHANNELS];
    double dvm_si[NUMCHANNELS];
    double zero[NUMCHANNELS];
    double dvm_si_zero[NUMCHANNELS];
    double dvm[NUMCHANNELS];

    double nominal_load[NUMCHANNELS];
    struct matrix coe;
    struct matrix mat;

    void initialize();

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
