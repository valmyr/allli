#include <Arduino.h>
#include <DS1307.h>
#include "Resources Files/dht.cpp"
#include "Resources Files/SensorPH.cpp"
#include "Resources Files/SensorCondutividade.cpp"
#include "Resources Files/sensorDeUmidadeSolo.cpp"
#define SensorPHPorte A0
#define SensorCondutividadeEletrica A1
#define SensorDeTemperaturaUmidade A2
#define SensorUmidadeDoSoloContinua A3
#define SensorUmidadeDoSoloGotejamento A4
#define relogio1 2
#define relogio2 3
#define IntervaloGotejamento 1 // 30 mim
#define AcionamentoBombaContinua 4
#define AcionamentoBombaGotejamento 5
#define umidadeDeCampo 500 //ideal
#define volumeDeAguaDaIrrigacao 1 //100 litros
#define vazaoDaBomba 900 // As bombas usadas tem uma vazão de 900 / h
//vazão =  dv / dt
float tempoQueAbombaFicaraLigada = float(volumeDeAguaDaIrrigacao)/float((float(vazaoDaBomba)/float(60*60)));//Em segundos
int n = 0;
long int tempoInicialBombaContinua;bool capituraContinuaTempo = true;
long int tempoInicialBombaGotejamentoEmMinutos,tempoInicialBombaGotejamento;bool capituraGotejamentoTempo = true;
int numeroDeLigacoesBombaGotejamento = 0;
SensorPH sensor(SensorPHPorte);
SensorCondutividade condutividadeEletrica(SensorCondutividadeEletrica);
SensorDeUmidadeSolo UmidadeSoloContinua(SensorUmidadeDoSoloContinua);
SensorDeUmidadeSolo UmidadeSoloGotejamento(SensorUmidadeDoSoloGotejamento);

dht DHT;
DS1307 relogio(relogio1, relogio2);

bool acionamentoDaBombaContinua(long int);
bool acionamentoDaBombaGotejamento(long int);

double temperaturaAmbiente(){
    delay(2000);
    return DHT.temperature;
}
double umidaderelativaDoAR(){
    delay(2000);
    return DHT.humidity;
}
void setup() {
    Serial.begin(9600);
    relogio.setDate(2,3,2021);
    relogio.setTime(2,0,0);
    pinMode(AcionamentoBombaContinua,OUTPUT);
    pinMode(AcionamentoBombaGotejamento,OUTPUT);
    digitalWrite(AcionamentoBombaContinua,LOW);
    digitalWrite(AcionamentoBombaGotejamento,LOW);
}
void loop() {
    DHT.read11(SensorDeTemperaturaUmidade);
    //********************************************************//
    if( UmidadeSoloContinua.getUmidade()>= float(umidadeDeCampo - umidadeDeCampo*.075)  and UmidadeSoloContinua.getUmidade()  < float(umidadeDeCampo + umidadeDeCampo*.075)){
        //Bloco Bomba Continua
        if(capituraContinuaTempo) {
            tempoInicialBombaContinua = millis();
            capituraContinuaTempo = false;
        }
        acionamentoDaBombaContinua(tempoInicialBombaContinua*pow(10,-3));   
    }else{
       capituraContinuaTempo = true;
    }
    if(digitalRead(AcionamentoBombaContinua)){
        Serial.println("Bomba Acionada Continua");
        
            acionamentoDaBombaContinua(tempoInicialBombaContinua*pow(10,-3));
    }else{
        Serial.println("Bomba parada Continua");
    }
    //**************************************************************//
}
bool acionamentoDaBombaContinua(long int tempoInicial){
    if(millis()*pow(10,-3)  < tempoQueAbombaFicaraLigada+tempoInicial){
        digitalWrite(AcionamentoBombaContinua,HIGH);
        return true;
    }
    digitalWrite(AcionamentoBombaContinua,LOW);
    return false;
}
bool acionamentoDaBombaGotejamento(long int tempoInicial){
    if(millis()*pow(10,-3) < float(tempoQueAbombaFicaraLigada/6.0) + tempoInicial){
        digitalWrite(AcionamentoBombaGotejamento,HIGH);
        return true;
    }
    digitalWrite(AcionamentoBombaGotejamento,LOW);
    return false;
}
