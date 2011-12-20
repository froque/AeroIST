#ifndef WIND_H
#define WIND_H

class Wind
{
public:
    Wind();
    ~Wind();
    void read(void);
    double wind;

protected:
    int arduinofd;

private:
    int wind_raw;
};

#endif // WIND_H
