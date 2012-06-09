#include "data.h"

Data::Data(int n){
    force = new QVector<double>[n];
    raw = new QVector<double>[n];
}
Data::~Data(){
    delete[] force;
    delete[] raw;
}
int Data::get_size(){
    return force[0].size();
}
double Data::get_value(int n,int row){
    return force[n].value(row);
}
QVector<double> Data::get_vector(int n){
    return force[n];
}
void Data::set_value(int n ,int row, double value) {
    force[n].replace(row,value);
}
void Data::insert_value(int n, int row, int count, double value){
    force[n].insert(row,count,value);
}
void Data::append_value(int n, double value) {
    force[n].append(value);
}
double Data::get_raw_value(int n,int row){
    return raw[n].value(row);
}
void Data::set_raw_value(int n ,int row, double value) {
    raw[n].replace(row,value);
}
void Data::insert_raw_value(int n, int row, int count, double value) {
    raw[n].insert(row,count,value);
}
void Data::append_raw_value(int n, double value) {
    raw[n].append(value);
}
void Data::set_zero(QVector<double> zero) {
    this->zero = zero;
}
QVector<double> Data::get_zero() {
    return zero;
}
