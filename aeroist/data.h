#ifndef DATA_H
#define DATA_H
#include <QVector>

class Data{
public:
    Data(int n);

    ~Data();
    int get_size();
    double get_value(int n,int row);
    QVector<double> get_vector(int n);
    void set_value(int n ,int row, double value);
    void insert_value(int n, int row, int count, double value);
    void append_value(int n, double value);
    double get_raw_value(int n,int row);
    void set_raw_value(int n ,int row, double value);
    void insert_raw_value(int n, int row, int count, double value);
    void append_raw_value(int n, double value);
    void set_zero(QVector<double> zero);
    QVector<double> get_zero();
private:
    QVector<double> *force;
    QVector<double> *raw;
    QVector<double> zero;
};

#endif // DATA_H
