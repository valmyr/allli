#include <Arduino.h>
#ifndef SensorPH_H
#define SensorPH_H
# define numeroDeColetas 10
class SensorPH{
private:
    int SensorPHPorte;
public:
    SensorPH(int);
    float getValorPH();
};
#endif