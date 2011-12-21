#ifndef TEMPERATURE_H
#define TEMPERATURE_H

class Temperature {
public:
    Temperature();
    ~Temperature();
    void read(void);
    double temp;

protected:
    int arduinofd;

private:
    int temp_raw;
};

#endif // TEMPERATURE_H
