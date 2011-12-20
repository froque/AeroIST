#ifndef COMMON_H
#define COMMON_H

#define DEBUG 1
//#define REAL_MEASURES 0
#define SINGLE_SHARED 0 // Don't test for open instances
#define SINGLE_PID 0
#define SINGLE_SETTINGS 0
#define SINGLE_SOCKET 0

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

#define PRECISION_ALPHA 0.03
#define PRECISION_BETA 0.05
#define COMMAND_ON 1
#define COMMAND_OFF 0

#define ARDUINO_ANALOG_REF 3.3
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
#define TAG_START "start"
#define TAG_END "end"
#define TAG_STEP "step"
#define TAG_CONTROL_TYPE "control_type"
#define TAG_N "n"
#define TAG_ZERO_ID "zero_id"
#define TAG_ZERO "zero"
#define TAG_MEASURE "measurement"
#define TAG_ITEM "item"
#define TAG_PROJECT "project"



#define SETTINGS_VIRTUAL_MEASURES "virtual_measures"
#define SETTINGS_PROJECT_FOLDER "project_folder"
#define SETTINGS_SCHEMA_FILE "schema_file"
#define SETTINGS_SCHEMA_FILE_DEFAULT "aeroist.xsd"
#define SETTINGS_GUI_STATE "gui/state"
#define SETTINGS_GUI_GEOMETRY "gui/geometry"
#define SETTINGS_GUI_TABWIDGET "gui/tabwidget"
#define SETTINGS_GUI_SPLITTERLISTS_STATE "gui/splitterLists/state"
#define SETTINGS_GUI_SPLITTERGLOBAL_STATE "gui/splitterGlobal/state"
#define SETTINGS_GUI_TABLECOLUMNSIZE "gui/tablecolumnsize"
#define SETTINGS_GUI_SHORTCUT "gui/shortcut/"
#define SETTINGS_GUI_ACTIONCHECKABLE "gui/actioncheckable/"
#define SETTINGS_DEFAULT_MATRIX "default_matrix"
#define SETTINGS_DEFAULT_DVM_TIME "default_dvm_time"
#define SETTINGS_DEFAULT_AVERAGE_NUMBER "default_average_number"
#define SETTINGS_DEFAULT_SETTLING_TIME "default_settling_time"
#define SETTINGS_FORCES_MATRIX_FLOOR "forces/matrix_floor"
#define SETTINGS_FORCES_MATRIX_FLOOR_DEFAULT "matrix 1.coe"
#define SETTINGS_FORCES_MATRIX_MIDDLE "forces/matrix_middle"
#define SETTINGS_FORCES_MATRIX_MIDDLE_DEFAULT "matrix 2.coe"
#define SETTINGS_MULTIMETER_PATH "multimeter_path"
#define SETTINGS_MULTIMETER_PATH_DEFAULT "voltmeter"
#define SETTINGS_ARDUINO_PATH "arduino_path"
#define SETTINGS_ARDUINO_PATH_DEFAULT "/dev/ttyUSB0"
#define SETTINGS_BETA_PATH "beta_path"
#define SETTINGS_BETA_PATH_DEFAULT "/dev/angle_beta"
#define SETTINGS_ALPHA_PATH "alpha_path"
#define SETTINGS_ALPHA_PATH_DEFAULT "/dev/angle_alpha"
#define SETTINGS_MOTOR_PATH "motor_path"
#define SETTINGS_MOTOR_PATH_DEFAULT "/dev/ttyS0"

#endif // COMMON_H


