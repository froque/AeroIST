#include "helper.h"

void Helper::msleep(int ms) {
    QThread::msleep(ms);
}

void Helper::sleep(int s) {
    QThread::sleep(s);
}
