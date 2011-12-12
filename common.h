#ifndef COMMON_H
#define COMMON_H

#define DEBUG 1
#define REAL_MEASURES 0
#define SINGLE_SHARED 0 // Don't test for open instances
#define SINGLE_PID 0
#define SINGLE_SETTINGS 0
#define SINGLE_SOCKET 0
#define INITIAL_LIST 0

#define AEROIST "AeroIST"

enum control_types_t{
    NONE=0,
    ALPHA,
    BETA,
    WIND
};

enum matrix_t{
    MIDDLE=0,
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

//#define MOTOR_DEV "/dev/ttyS0"
#define WIND_FULLSPEED 16384.0 // 100% according to manual
#define WIND_FULLPERCENTAGE 100.0



#ifdef DEBUG
#include <QDebug>
#endif //DEBUG


#define TAG_NAME "name"
#define TAG_ID "id"
#define TAG_DESCRIPTION "description"
#define TAG_DVM_TIME "dvm_time"
#define TAG_MATRIX "matrix"
#define TAG_AVERAGE_NUMBER "average_number"
#define TAG_SET_ALPHA "set_alpha"
#define TAG_SET_BETA "set_beta"
#define TAG_SET_WIND "set_wind"
#define TAG_DATA "data"
#define TAG_SETTLING_TIME "settling_time"
#define TAG_MIN "min"
#define TAG_MAX "max"
#define TAG_STEP "step"
#define TAG_CONTROL_TYPE "control_type"
#define TAG_N "n"
#define TAG_ZERO_ID "zero_id"
#define TAG_ZERO "zero"
#define TAG_MEASURE "measurement"
#define TAG_ITEM "item"
#define TAG_PROJECT "project"
#endif // COMMON_H


