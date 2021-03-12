#include "../Header Files/SensorPH.h"
SensorPH::SensorPH(int SensorPHPorte) {
    this->SensorPHPorte = SensorPHPorte;
    pinMode(this->SensorPHPorte,INPUT);
}
float SensorPH::getValorPH(){
    float coletas[numeroDeColetas];
    int  i = 0;
    for(i = 0; i < numeroDeColetas; i++) {
        coletas[i] = float(analogRead(SensorPHPorte));
    }
    //Ordenando
    while(true){
        for(int j = 1; j < i--; j++){
            if(coletas[j-1] < coletas[j]){
                int valorTemporario = coletas[j];
                coletas[j] = coletas[j-1];
                coletas[j+1] = valorTemporario;
            }
        }
        if(i -1 >= numeroDeColetas)
          break;
    }
    float media = 0;
    for(int i = 2; i < numeroDeColetas-2; i++){
        media+=coletas[i];
    }
    return -0.0308*media+ 23.059;
}
