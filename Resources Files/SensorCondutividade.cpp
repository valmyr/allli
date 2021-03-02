#include "../Header Files/SensorCondutividade.h"

SensorCondutividade::SensorCondutividade(int condutividadePorte){
    this->condutividadePorte = condutividadePorte;
    pinMode(this->condutividadePorte, INPUT);
}
float SensorCondutividade::getCondutividadeDoSolo(){
    return float(analogRead(this->condutividadePorte));
}
