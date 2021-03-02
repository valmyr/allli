#include <Arduino.h>
#ifndef SensorCondutividade_H
#define SensorCondutividade_H
class SensorCondutividade{
    private:
        int condutividadePorte;
    public:
        SensorCondutividade(int);
        float getCondutividadeDoSolo();
};
#endif