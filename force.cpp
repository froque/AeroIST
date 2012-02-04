#include "force.h"

#include <cstdio>
#include <gpib/ib.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <QString>
#include <QSettings>
#include <stdexcept>

void invert(int n,double coe_matrix[6][6],double matrix[6][6]);

// initialize the class
Force::Force(matrix_t matrix, int dvm_time,double zero[NUMCHANNELS]):
    dvm_time(dvm_time),
    matrix(matrix)
{
    for(int k=0; k< NUMCHANNELS;k++){
        this->zero[k] = zero[k];
    }
    initialize();
}
Force::Force(matrix_t matrix, int dvm_time):
    dvm_time(dvm_time),
    matrix(matrix)
{
    for(int k=0; k< NUMCHANNELS;k++){
        this->zero[k] = 0;
    }
    initialize();
}
void Force::initialize(){
    QSettings settings;
    if (matrix == FLOOR){
        filename = settings.value(SETTINGS_FORCES_MATRIX_FLOOR,SETTINGS_FORCES_MATRIX_FLOOR_DEFAULT).toString();
    } else{
        filename = settings.value(SETTINGS_FORCES_MATRIX_MIDDLE,SETTINGS_FORCES_MATRIX_MIDDLE_DEFAULT).toString();
    }
    std::ifstream matrix_file(filename.toStdString().c_str(),std::ios::in|std::ios::binary);
    if (matrix_file.is_open()) {
        matrix_file.read((char*)&coe, sizeof(struct matrix));
        matrix_file.close();
    } else {
        throw std::runtime_error("Could not open matrix file");
    }
    invert(NUMCHANNELS,coe.coef_lin,mat.coef_lin);

    nominal_load[0] = 50.0;         // why? well it was calibrated this way.
    nominal_load[1] = 100.0;
    nominal_load[2] = 100.0;
    nominal_load[3] = 100.0;
    nominal_load[4] = 100.0;
    nominal_load[5] = 100.0;

    int row,j,line,k;

    // for getting the dvm for the zero. using R = AF + BF^2
    for (row = 0; row< NUMCHANNELS; row++){
        k = 0;
        dvm_si_zero[row] = 0.0;
        for (line = 0; line < NUMCHANNELS; line++){
            dvm_si_zero[row] = dvm_si_zero[row] + coe.coef_lin[line][row] * zero[line];  //matrix is transposed on file
            for (j = line; j < NUMCHANNELS; j++){
                dvm_si_zero[row] = dvm_si_zero[row] + (coe.coef_qua[k][row] * zero[j] * zero[line]); //matrix is transposed on file
                k++;
            }
        }
    }

    g_id = ibfind(settings.value(SETTINGS_MULTIMETER_PATH).toString().toStdString().c_str());
    if (  g_id == -1 ){
        throw std::runtime_error("unable to open GPIB device");
    }
}

// destroy the class
Force::~Force(){
    ibonl(g_id,0);
}

// public function. Read from the dmm and returns the forces
void Force::read(void){
    read_dvm();             // determines dvm
    convert_dvm();        // determines dvm_si
    newton_method();      // determines forces
}


// convert mV to Newtons
void Force::convert_dvm(){
    for (int k=0; k < NUMCHANNELS; k++ ){
        dvm_si[k] =  nominal_load[k] * GRAVITY * dvm[k] * 1000 / (SENSITIVITY * VCC);
    }
}

// read ASCII from the DMM
double  Force::ascii2newton (char *buf){
    double value;
    int status;
    status = sscanf(buf,"%le", &value);
    if (status != 1){
        printf("sscanf: %s\n", buf);
        throw std::runtime_error("problem reading from GPIB device");
    }
    return value;
}

void Force::read_dvm(void){
    char buf[READBUFFER];
    QString dvmstr = QString("M0VDR1A0T%1L0").arg(dvm_time);

    for (int k=0; k < NUMCHANNELS; k++ ){
        dvmstr.replace(1,1,QString::number(k));
        ibwrt(g_id,dvmstr.toStdString().c_str(),dvmstr.size());
        ibrd(g_id,buf, READBUFFER);
        dvm[k] = ascii2newton(buf);
    }
}


void Force::newton_method( ){

    double F[NUMCHANNELS];
    double jm[NUMCHANNELS][NUMCHANNELS];
    double jm_inv[NUMCHANNELS][NUMCHANNELS];

    //initial guess
    for(int i=0;i<NUMCHANNELS;i++){
        forces[i]=0;
        for(int j=0;j<NUMCHANNELS;j++){
            forces[i] += mat.coef_lin[j][i] * (dvm_si[j] - dvm_si_zero[j]);   //note: matrix is transposed on file
        }
    }

    unsigned int iter=0;
    do {
        calc_jacobi(forces,jm);          // jacobian matrix
        invert(NUMCHANNELS,jm,jm_inv);          // jacobian inverted
        calc_function( F);  // calculate function F(x)=0
        for (int i=0;i<NUMCHANNELS;i++){
            for(int j=0;j<NUMCHANNELS;j++){
                forces[i] -= jm_inv[i][j]*F[j];  // x_n+1 = x_n - J^-1 * F(x_n)
            }
        }
        iter++;
    } while( iter < MAX_ITER && check_tolerance(F));   // stop on number reached or convergence
}

void Force::calc_function(double F[NUMCHANNELS])  //calculates THE function F(x)=0; => A.F + B.F^2 - R = 0
{
    int row,j,line,k;

    for (row = 0; row< NUMCHANNELS; row++){
        k = 0;
        F[row] = 0.0;
        for (line = 0; line < NUMCHANNELS; line++){
            F[row] = F[row] + coe.coef_lin[line][row] * forces[line];  //matrix is transposed on file
            for (j = line; j < NUMCHANNELS; j++){
                F[row] = F[row] + (coe.coef_qua[k][row] * forces[j] * forces[line]); //matrix is transposed on file
                k++;
            }
        }
        F[row] = F[row] - (dvm_si[row] - dvm_si_zero[row]);
    }
}

bool Force::check_tolerance(double F[NUMCHANNELS]){
    bool in_tolerance=true;
    for (int i=0;i<NUMCHANNELS;i++){
        if (F[i] > TOLERANCE){
            in_tolerance=false;
        }
    }
    return in_tolerance;
}


void invert(int n,double coe_matrix[6][6],double matrix[6][6])
{
    register int i,j,k;
    int v1[6];		/* hier ist die groesse fest !!! */
    int v2[6];		/* hier ist die groesse fest !!! */
    double g,h;

    /* matrix umcopieren in ergebnisfeld */

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            matrix[i][j] = coe_matrix[i][j];
        }
    }

    /* suche nach dem groessten element */

    for (k = 0; k < n; k++)
    {
        v1[k] = k;
        v2[k] = k;
        g = matrix[k][k];
        for (j = k; j < n; j++)
        {
            for (i = k; i < n; i++)
            {
                if (( fabs(g) - fabs(matrix[i][j])) < 0)
                {
                    g = matrix[i][j];
                    v1[k] = i;
                    v2[k] = j;
                }	/* end if */
            }	/* end for i */
        }		/* end for j */
        j = v1[k];
        if ((j-k) > 0)
        {
            for (i = 0; i < n; i++)
            {
                h = -matrix[k][i];
                matrix[k][i] = matrix[j][i];
                matrix[j][i] = h;
            }  /* end for i */
        }	   /* end if */

        /* vertauschen der spalten */

        i = v2[k];
        if ((i-k) > 0)
        {
            for (j = 0; j < n; j++)
            {
                h = -matrix[j][k];
                matrix[j][k] = matrix[j][i];
                matrix[j][i] = h;
            }   /* end for j */
        }	    /* end if */

        /* division der spalte durch invertiertes
            pivotelement */

        if ( g != 0)
        {
            for (i = 0; i < n; i++)
            {
                if ((i-k) != 0)
                {
                    matrix[i][k] = matrix[i][k]/(-g);
                }	/* end if */
            }	/* end for i */

            /* reduktion der matrix */

            for (i = 0; i < n; i++)
            {
                h = matrix[i][k];
                for (j = 0; j < n; j++)
                {
                    if (((i-k) != 0) && ((j-k) != 0))
                    {
                        matrix[i][j] = h * matrix[k][j] +
                                matrix[i][j];
                    }	/* end if */
                }		/* end for j */
            }		/* end for i */

            /* division der zeile durch das pivotelement */

            for (j = 0; j < n; j++)
            {
                if ((j-k) != 0)
                {
                    matrix[k][j] = matrix[k][j]/g;
                }	/* end if */
            }	/* end for j */

            /* rez. wert des pivotelements */

            matrix[k][k] = 1/g;
        }	/* end if g != 0 */
    }	/* end for k */

    /* letzter zeilen und spaltentausch */

    k = n;
    while (k > 0)
    {
        k = k -1;
        i = v1[k];
        if ((i-k) > 0)
        {
            for (j = 0; j < n; j++)
            {
                h = matrix[j][k];
                matrix[j][k] = -matrix[j][i];
                matrix[j][i] = h;
            }	/* end for j */
        }	/* end if */
        j = v2[k];
        if ((j - k) > 0)
        {
            for (i = 0; i < n; i++)
            {
                h = matrix[k][i];
                matrix[k][i] = -matrix[j][i];
                matrix[j][i] = h;
            }	/* end for i */
        }	/* end if */
    }
}

void Force::calc_jacobi(double force[],double jm[NUMCHANNELS][NUMCHANNELS]){
    int i,j;
    static int k[NUMCHANNELS][ANZ_QUA_ELE] = {
        /* k1k1 k1k2 k1k3 k1k4 k1k5 k1k6 k2k2 k2k3 k2k4 k2k5 k2k6 k3k3 k3k4 k3k5 k3k6 k4k4 k4k5 k4k6 k5k5 k5k6 k6k6 */
        {   2 ,  1 ,  1 ,  1 ,	1 ,  1 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 },
        {   0 ,  1 ,  0 ,  0 ,	0 ,  0 ,  2 ,  1 ,  1 ,  1 ,  1 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 },
        {   0 ,  0 ,  1 ,  0 ,	0 ,  0 ,  0 ,  1 ,  0 ,  0 ,  0 ,  2 ,  1 ,  1 ,  1 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 },
        {   0 ,  0 ,  0 ,  1 ,	0 ,  0 ,  0 ,  0 ,  1 ,  0 ,  0 ,  0 ,  1 ,  0 ,  0 ,  2 ,  1 ,  1 ,  0 ,  0 ,  0 },
        {   0 ,  0 ,  0 ,  0 ,	1 ,  0 ,  0 ,  0 ,  0 ,  1 ,  0 ,  0 ,  0 ,  1 ,  0 ,  0 ,  1 ,  0 ,  2 ,  1 ,  0 },
        {   0 ,  0 ,  0 ,  0 ,	0 ,  1 ,  0 ,  0 ,  0 ,  0 ,  1 ,  0 ,  0 ,  0 ,  1 ,  0 ,  0 ,  1 ,  0 ,  1 ,  2 }
    };

    for (j = 0; j< NUMCHANNELS; j++)
    {
        for (i = 0; i< NUMCHANNELS; i++)
        {
            jm[i][j] = coe.coef_lin[j][i];
            jm[i][j] = jm[i][j] +
                    ( coe.coef_qua[0][i] * force[0] * k[j][0] ) +
                    ( coe.coef_qua[1][i] * force[1] * k[j][1] ) +
                    ( coe.coef_qua[2][i] * force[2] * k[j][2] ) +
                    ( coe.coef_qua[3][i] * force[3] * k[j][3] ) +
                    ( coe.coef_qua[4][i] * force[4] * k[j][4] ) +
                    ( coe.coef_qua[5][i] * force[5] * k[j][5] ) +

                    ( coe.coef_qua[6][i] * force[1] * k[j][6] ) +
                    ( coe.coef_qua[7][i] * force[2] * k[j][7] ) +
                    ( coe.coef_qua[8][i] * force[3] * k[j][8] ) +
                    ( coe.coef_qua[9][i] * force[4] * k[j][9] ) +
                    ( coe.coef_qua[10][i] * force[5] * k[j][10] ) +

                    ( coe.coef_qua[11][i] * force[2] * k[j][11] ) +
                    ( coe.coef_qua[12][i] * force[3] * k[j][12] ) +
                    ( coe.coef_qua[13][i] * force[4] * k[j][13] ) +
                    ( coe.coef_qua[14][i] * force[5] * k[j][14] ) +

                    ( coe.coef_qua[15][i] * force[3] * k[j][15] ) +
                    ( coe.coef_qua[16][i] * force[4] * k[j][16] ) +
                    ( coe.coef_qua[17][i] * force[5] * k[j][17] ) +

                    ( coe.coef_qua[18][i] * force[4] * k[j][18] ) +
                    ( coe.coef_qua[19][i] * force[5] * k[j][19] ) +

                    ( coe.coef_qua[20][i] * force[5] * k[j][20] );
        }
    }
}


QString Force::get_name(int n) {
    switch (n){
        case 0: return "Fx"; break;
        case 1: return "Fy"; break;
        case 2: return "Fz"; break;
        case 3: return "Mx"; break;
        case 4: return "My"; break;
        case 5: return "Mz"; break;
    }
}
