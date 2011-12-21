#ifndef HELPER_H
#define HELPER_H

#include <QThread>

class Helper: public QThread {
public:
    static void msleep(int ms);
    static void sleep(int s);
};
#endif // HELPER_H
