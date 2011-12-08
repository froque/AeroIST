#ifndef COMMON_H
#define COMMON_H

#define DEBUG 1
#define SINGLE_SHARED 0 // Don't test for open instances
#define SINGLE_PID 0
#define SINGLE_SETTINGS 0
#define SINGLE_SOCKET 0
#define INITIAL_LIST 0

#define AEROIST "AeroIST"

enum control_types_t{
    NONE,
    ALPHA,
    BETA,
    WIND
};

enum matrix_t{
    MIDDLE,
    FLOOR
};

enum ThreadStatus{
    STOPPED,
    MEASURE_RUNNING,
    ZERO_RUNNING
};


//#define DEFAULT_ALPHA_MIN   -30
//#define DEFAULT_ALPHA_MAX   30
#define DEFAULT_ALPHA_STEP  1

//#define DEFAULT_BETA_MIN    -170
//#define DEFAULT_BETA_MAX    170
#define DEFAULT_BETA_STEP   1

#define DEFAULT_WIND_MIN    0
#define DEFAULT_WIND_MAX    100 //Percentage
#define DEFAULT_WIND_STEP   1


#define ANGLEZERO_ALPHA 730303.0
#define ANGLEZERO_BETA 80751.0
#define ANGLESENSITIVITY_ALPHA 0.0000009007
#define ANGLESENSITIVITY_BETA 0.00211000

#define ANGLEMAX_ALPHA 30.0
#define ANGLEMAX_BETA 180.0

#define PRECISION 0.03
//#define ARDUINOPATH "/dev/ttyUSB0"


#ifdef DEBUG
#include <QDebug>
#endif //DEBUG

#endif // COMMON_H
