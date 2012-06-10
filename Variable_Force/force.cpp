#include "force.h"

#include <gpib/ib.h>
#include <QString>
#include <QSettings>
#include <stdexcept>

#define GRAVITY 9.81      /* m/s^2 */
#define SENSITIVITY 2.0   /* mV/V  */
#define VCC 12            /* V     */
#define MAX_ITER 10
#define TOLERANCE 1e-5
#define READBUFFER 12     /* L0 - short message */

#define SETTINGS_DEFAULT_MATRIX "default_matrix"
#define SETTINGS_DEFAULT_DVM_TIME "default_dvm_time"
#define SETTINGS_FORCES_MATRIX_FLOOR "forces/matrix_floor"
#define SETTINGS_FORCES_MATRIX_FLOOR_DEFAULT "matrix 1.coe"
#define SETTINGS_FORCES_MATRIX_MIDDLE "forces/matrix_middle"
#define SETTINGS_FORCES_MATRIX_MIDDLE_DEFAULT "matrix 2.coe"
#define SETTINGS_MULTIMETER_PATH "multimeter_path"
#define SETTINGS_MULTIMETER_PATH_DEFAULT "voltmeter"
#define SETTINGS_MATRIX_ALLOW "matrix_allow"

#define TAG_DVM_TIME "dvm_time"
#define TAG_MATRIX "matrix"

void invert(int n,double coe_matrix[6][6],double matrix[6][6]);

bool ForceMeta::is_controlable() {
    return false;
}
bool ForceMeta::has_zero() {
    return true;
}
int ForceMeta::get_num() {
    return 6;
}
QString ForceMeta::get_general_name(){
    return "Forces";
}
QString ForceMeta::get_general_name_tr(){
    return QString(QObject::tr("Forces"));
}
QString ForceMeta::get_name(int n){
    switch (n){
    case 0: return "Fx"; break;
    case 1: return "Fy"; break;
    case 2: return "Fz"; break;
    case 3: return "Mx"; break;
    case 4: return "My"; break;
    case 5: return "Mz"; break;
    }
    return "";
}
QString ForceMeta::get_name_tr(int n){
    switch (n){
    case 0: return QString(QObject::tr("Fx")); break;
    case 1: return QString(QObject::tr("Fy")); break;
    case 2: return QString(QObject::tr("Fz")); break;
    case 3: return QString(QObject::tr("Mx")); break;
    case 4: return QString(QObject::tr("My")); break;
    case 5: return QString(QObject::tr("Mz")); break;
    }
    return "";
}
QString ForceMeta::get_units(int n) {
    switch (n){
    case 0:
    case 1:
    case 2:
        return QString(QObject::tr("N")); break;
    case 3:
    case 4:
    case 5:
        return QString(QObject::tr("N.m")); break;
    }
    return "";
}
QString ForceMeta::get_raw_units(int n) {
    Q_UNUSED(n);
    return QString(QObject::tr("V"));
}
double ForceMeta::get_lower_bound(int n) {
    Q_UNUSED(n);
    return 0;
}
double ForceMeta::get_upper_bound(int n) {
    Q_UNUSED(n);
    return 0;
}
double ForceMeta::get_smaller_step(int n) {
    Q_UNUSED(n);
    return 0;
}
double ForceMeta::get_default_step(int n) {
    Q_UNUSED(n);
    return 0;
}
double ForceMeta::get_default_start(int n) {
    Q_UNUSED(n);
    return 0;
}


ForcePreferences::ForcePreferences(){
}
ForcePreferences::~ForcePreferences(){
}
QWidget* ForcePreferences::get_widget() {
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    layout->addWidget(new QLabel(QObject::tr("Multimeter")),0,0);
    edit = new QLineEdit(settings.value(SETTINGS_MULTIMETER_PATH, SETTINGS_MULTIMETER_PATH_DEFAULT).toString());
    layout->addWidget(edit,0,1);
    layout->addWidget(new QLabel(QObject::tr("Default Multimeter time")),1,0);
    combo_time = new QComboBox;
    combo_time->addItem(QObject::tr("50 ms"),    1);
    combo_time->addItem(QObject::tr("100 ms"),   2);
    combo_time->addItem(QObject::tr("500 ms"),   3);
    combo_time->addItem(QObject::tr("1 s"),      4);
    combo_time->addItem(QObject::tr("5 s"),      5);
    combo_time->addItem(QObject::tr("10 s"),     6);
    int index = combo_time->findData(settings.value(SETTINGS_DEFAULT_DVM_TIME,4).toInt());
    combo_time->setCurrentIndex(index);
    layout->addWidget(combo_time,1,1);
    layout->addWidget(new QLabel(QObject::tr("Default Matrix")),2,0);
    combo_matrix =  new QComboBox;
    combo_matrix->addItem(QObject::tr("Middle"), MIDDLE);
    combo_matrix->addItem(QObject::tr("Floor"),  FLOOR);
    index = combo_matrix->findData(settings.value(SETTINGS_DEFAULT_MATRIX,MIDDLE).toInt());
    combo_matrix->setCurrentIndex(index);
    layout->addWidget(combo_matrix,2,1);

    checkbox = new QCheckBox(QObject::tr("Allow different matrixes between measurement and reference"));
    checkbox->setChecked(settings.value(SETTINGS_MATRIX_ALLOW,false).toBool());
    layout->addWidget(checkbox,3,0,1,2);

    widget->setLayout(layout);
    return widget;
}
bool ForcePreferences::accept_config() {
    QSettings settings;
    int index = combo_time->currentIndex();
    settings.setValue(SETTINGS_DEFAULT_DVM_TIME, combo_time->itemData(index).toInt());
    settings.setValue(SETTINGS_MULTIMETER_PATH, edit->text());
    index = combo_matrix->currentIndex();
    settings.setValue(SETTINGS_DEFAULT_MATRIX, combo_matrix->itemData(index).toInt());
    settings.setValue(SETTINGS_MATRIX_ALLOW,checkbox->isChecked());
    return true;
}
bool ForcePreferences::is_configurable() {
    return true;
}


ForceModel::ForceModel(){
}
ForceModel::~ForceModel(){
}
QWidget* ForceModel::view_get_widget(){
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(QObject::tr("Multimeter time")),0,0);
    layout->addWidget(new QLabel(),0,1);

    switch (dvm_time){
    case 1:
        layout->addWidget(new QLabel(QObject::tr("50 ms")),0,1); break;
    case 2:
        layout->addWidget(new QLabel(QObject::tr("100 ms")),0,1); break;
    case 3:
        layout->addWidget(new QLabel(QObject::tr("500 ms")),0,1); break;
    case 4:
        layout->addWidget(new QLabel(QObject::tr("1 s")),0,1); break;
    case 5:
        layout->addWidget(new QLabel(QObject::tr("5 s")),0,1); break;
    case 6:
        layout->addWidget(new QLabel(QObject::tr("10 s")),0,1); break;
    }
    layout->addWidget(new QLabel(QObject::tr("Test Type")),1,0);
    switch (matrix){
    case MIDDLE:
        layout->addWidget(new QLabel(QObject::tr("Middle")),1,1); break;
    case FLOOR:
        layout->addWidget(new QLabel(QObject::tr("Floor")),1,1); break;
    }
    layout->addWidget(new QLabel(),1,1);
    widget->setLayout(layout);
    return widget;
}
QWidget* ForceModel::measurement_get_widget(){
    QWidget *widget = new QWidget;
    QGridLayout *layout = new QGridLayout;
    QSettings settings;
    layout->addWidget(new QLabel(QObject::tr("Multimeter time")),0,0);
    combo_time = new QComboBox;
    combo_time->addItem(QObject::tr("50 ms"),    1);
    combo_time->addItem(QObject::tr("100 ms"),   2);
    combo_time->addItem(QObject::tr("500 ms"),   3);
    combo_time->addItem(QObject::tr("1 s"),      4);
    combo_time->addItem(QObject::tr("5 s"),      5);
    combo_time->addItem(QObject::tr("10 s"),     6);
    int index = combo_time->findData(settings.value(SETTINGS_DEFAULT_DVM_TIME).toInt());
    combo_time->setCurrentIndex(index);
    layout->addWidget(combo_time,0,1);
    layout->addWidget(new QLabel(QObject::tr("Test Type")),1,0);
    combo_matrix =  new QComboBox;
    combo_matrix->addItem(QObject::tr("Middle"), MIDDLE);
    combo_matrix->addItem(QObject::tr("Floor"),  FLOOR);
    index = combo_matrix->findData(settings.value(SETTINGS_DEFAULT_MATRIX).toInt());
    combo_matrix->setCurrentIndex(index);
    layout->addWidget(combo_matrix,1,1);
    widget->setLayout(layout);
    return widget;
}
bool ForceModel::measurement_accept_config(VariableModel *m){
    dvm_time = combo_time->itemData(combo_time->currentIndex()).toInt();
    matrix = (matrix_t) combo_matrix->currentIndex();
    QSettings settings;
    bool allow = settings.value(SETTINGS_MATRIX_ALLOW,false).toBool();
    if (m != NULL && allow == false){
        if (matrix != dynamic_cast<ForceModel*>(m)->matrix){
            QMessageBox message;
            message.setText("The reference matrix and the measurement matrix are different.");
            message.exec();
            return false;
        }
    }
    return true;
}
bool ForceModel::measurement_is_configurable(){
    return true;
}
void ForceModel::save_xml(QDomElement root){
    QDomElement dvm_time = root.ownerDocument().createElement(TAG_DVM_TIME);
    dvm_time.appendChild(root.ownerDocument().createTextNode(QString::number(this->dvm_time)));
    root.appendChild(dvm_time);

    QDomElement matrix = root.ownerDocument().createElement(TAG_MATRIX);
    matrix.appendChild(root.ownerDocument().createTextNode(QString::number(this->matrix)));
    root.appendChild(matrix);
}
void ForceModel::load_xml(QDomElement root){
    QDomNodeList nodelist = root.childNodes();
    QDomNode node;
    QDomElement element;
    for (int k=0; k< nodelist.count();k++){
        node = nodelist.at(k);
        element = node.toElement();


        if (element.tagName() == TAG_DVM_TIME){
            this->dvm_time = element.text().toInt();
            continue;
        }
        if (element.tagName() == TAG_MATRIX){
            int m = element.text().toInt();
            switch (m){
            case FLOOR: this->matrix = FLOOR; break;
            case MIDDLE: this->matrix = MIDDLE; break;
            }
            continue;
        }
    }
}


ForceHardware::ForceHardware(VariableModel* v){
    dvm_time = dynamic_cast<ForceModel*>(v)->dvm_time;
    matrix = dynamic_cast<ForceModel*>(v)->matrix;

    zero = QVector<double>(NUMCHANNELS,0);

    QSettings settings;
     if (matrix == FLOOR){
         filename = settings.value(SETTINGS_FORCES_MATRIX_FLOOR,SETTINGS_FORCES_MATRIX_FLOOR_DEFAULT).toString();
     } else{
         filename = settings.value(SETTINGS_FORCES_MATRIX_MIDDLE,SETTINGS_FORCES_MATRIX_MIDDLE_DEFAULT).toString();
     }
     // matrix are in the same path as executable
     filename.prepend(qApp->applicationDirPath() + "/");
     QFile matrix_file(filename);
     if(matrix_file.open(QIODevice::ReadOnly) == true){
         matrix_file.read((char*)&coe,sizeof(struct matrix));
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

     for (int row = 0; row< NUMCHANNELS; row++){
         dvm_si_zero[row] = 0.0;
     }

     g_id = ibfind(settings.value(SETTINGS_MULTIMETER_PATH,SETTINGS_MULTIMETER_PATH_DEFAULT).toString().toStdString().c_str());
     if (  g_id == -1 ){
         throw std::runtime_error("unable to open GPIB device");
     }
}

ForceHardware::~ForceHardware(){
    ibonl(g_id,0);
}
void ForceHardware::read() {
    read_dvm();             // determines dvm
    convert_dvm();        // determines dvm_si
    newton_method();      // determines forces
}

double ForceHardware::get_value(int n) {
    return forces[n];
}
void ForceHardware::set_value(int n ,double value) {
    Q_UNUSED(n);
    Q_UNUSED(value);
}
double ForceHardware::get_raw_value(int n){
    return dvm[n];
}
bool ForceHardware::isReady(void) {
    char buf[READBUFFER];
    ibwrt(g_id,"M0VDR1A0T1L0",12);
    ibrd(g_id,buf, READBUFFER);
    if (ibcnt == 12 ){
        return true;
    } else {
        return false;
    }
}
bool ForceHardware::has_set_final() {
    return meta->is_controlable() && false;
}
void ForceHardware::set_final() {
}
void ForceHardware::set_zero(QVector<double> zero) {
    this->zero = zero;

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
}

void ForceHardware::convert_dvm(){
    for (int k=0; k < NUMCHANNELS; k++ ){
        dvm_si[k] =  nominal_load[k] * GRAVITY * dvm[k] * 1000 / (SENSITIVITY * VCC);
    }
}

double  ForceHardware::ascii2newton (char *buf){
    double value;
    int status;
    status = sscanf(buf,"%le", &value);
    if (status != 1){
        printf("sscanf: %s\n", buf);
        throw std::runtime_error("problem reading from GPIB device");
    }
    return value;
}

void ForceHardware::read_dvm(void){
    char buf[READBUFFER];
    QString dvmstr = QString("M0VDR1A0T%1L0").arg(dvm_time);

    for (int k=0; k < NUMCHANNELS; k++ ){
        dvmstr.replace(1,1,QString::number(k));
        ibwrt(g_id,dvmstr.toStdString().c_str(),dvmstr.size());
        ibrd(g_id,buf, READBUFFER);
        dvm[k] = ascii2newton(buf);
    }
}


void ForceHardware::newton_method( ){

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

void ForceHardware::calc_function(double F[NUMCHANNELS])  //calculates THE function F(x)=0; => A.F + B.F^2 - R = 0
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

bool ForceHardware::check_tolerance(double F[NUMCHANNELS]){
    bool in_tolerance=true;
    for (int i=0;i<NUMCHANNELS;i++){
        if (F[i] > TOLERANCE){
            in_tolerance=false;
        }
    }
    return in_tolerance;
}

void ForceHardware::calc_jacobi(double force[],double jm[NUMCHANNELS][NUMCHANNELS]){
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

VariableMeta* ForceFactory::CreateVariableMeta() {
    return new ForceMeta;
}
VariablePreferences* ForceFactory::CreateVariableGUI() {
    return new ForcePreferences;
}
VariableModel* ForceFactory::CreateVariableModel() {
    return new ForceModel;
}
VariableHardware* ForceFactory::CreateVariableHardware(VariableModel *v) {
    return new ForceHardware(v);
}

Q_EXPORT_PLUGIN2(10_force, ForceFactory);
