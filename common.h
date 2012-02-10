#ifndef COMMON_H
#define COMMON_H

#define DEBUG 1

#define SINGLE_SHARED 0 // Don't test for open instances
#define SINGLE_PID 0
#define SINGLE_SETTINGS 0
#define SINGLE_SOCKET 0

#define AEROIST "AeroIST"

enum matrix_t{
    MIDDLE=0,
    FLOOR
};

enum ThreadStatus{
    STOPPED,
    MEASURE_RUNNING,
    ZERO_RUNNING
};


#define DEFAULT_ALPHA_STEP  1
#define DEFAULT_BETA_STEP   5
#define DEFAULT_ALPHA_START 0
#define DEFAULT_BETA_START  0

#define MOTOR_MIN    0
#define MOTOR_MAX    100 //Percentage
#define MOTOR_MIN_STEP 1
#define DEFAULT_MOTOR_STEP   1

#define ANGLEZERO_ALPHA 730303.0
#define ANGLEZERO_BETA 80751.0
#define ANGLESENSITIVITY_ALPHA 0.0000009007
#define ANGLESENSITIVITY_BETA 0.00211000

#define ANGLEMAX_ALPHA 30.0
#define ANGLEMAX_BETA 180.0
#define ANGLEMINSTEP_ALPHA 0.1
#define ANGLEMINSTEP_BETA 0.2

#define PRECISION_ALPHA 0.03
#define PRECISION_BETA 0.05
#define COMMAND_ON 1
#define COMMAND_OFF 0

#define ARDUINO_ANALOG_REF 5.0
#define MOTOR_FULLSPEED 16384.0 // 100% according to manual
#define MOTOR_FULLPERCENTAGE 100.0
#define MOTOR_PRECISION 2           // in percentage
#define TEMPERATURE_SENSITIVITY 100.0
#define WIND_SENSITIVITY 3.98
#define MMH2O_TO_PASCAL 9.80665

#define GRAVITY 9.81      /* m/s^2 */
#define SENSITIVITY 2.0   /* mV/V  */
#define VCC 12            /* V     */
#define MAX_ITER 10
#define TOLERANCE 1e-5



#define TAG_NAME "name"
#define TAG_DESCRIPTION "description"
#define TAG_DVM_TIME "dvm_time"
#define TAG_MATRIX "matrix"
#define TAG_AVERAGE_NUMBER "average_number"
#define TAG_DATA "data"
#define TAG_SETTLING_TIME "settling_time"
#define TAG_START_VALUES "start_values"
#define TAG_START "start"
#define TAG_END "end"
#define TAG_STEP "step"
#define TAG_CONTROL_TYPE "control_type"
#define TAG_N "n"
#define TAG_DATA_ZERO "data_zero"
#define TAG_ZERO "zero"
#define TAG_ZERO_NAME "zero_name"
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
