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
#define SensorUmidadeDoSoloGotijamento A4
#define relogio1 2
#define relogio2 3
#define AcionamentoBombaContinua 4
#define AcionamentoBombaGotejamento 5
#define umidadeDeCampo 500 //ideal
#define volumeDeAguaDaIrrigacao 10 //10 litros
#define vazaoDaBomba 900 // As bombas usadas tem uma vazão de 900 / h

//vazão =  dv / dt
float tempoQueAbombaFicaraLigada = float(volumeDeAguaDaIrrigacao)/float((vazaoDaBomba/(60*60)));//Em segundos
long int tempoInicialBombaContinua;bool capituraContinuaTempo = true;
long int tempoInicialBombaGotijamentoEmMinutos,tempoInicialBombaGotijamento;bool capituraGotijamentoTempo = true;
int numeroDeLigacoesBombaGotijamento = 0;
SensorPH sensor(SensorPHPorte);
SensorCondutividade condutividadeEletrica(SensorCondutividadeEletrica);
SensorDeUmidadeSolo UmidadeSoloContinua(SensorUmidadeDoSoloContinua);
SensorDeUmidadeSolo UmidadeSoloGotijamento(SensorUmidadeDoSoloGotijamento);

dht DHT;
DS1307 relogio(relogio1, relogio2);

bool acionamentoDaBombaContinua(long int);
bool acionamentoDaBombaGotijamento(long int);

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
    Time DataHora = relogio.getTime();

    if(float(umidadeDeCampo - umidadeDeCampo*.075) <  UmidadeSoloContinua.getUmidade() < float(umidadeDeCampo + umidadeDeCampo*.075)){
        //Bloco Bomba Continua
        if(capituraContinuaTempo) {
            tempoInicialBombaContinua = micros();
            capituraContinuaTempo = false;
        }else{
            if(digitalRead(AcionamentoBombaContinua))
                tempoInicialBombaContinua = micros();
            }
            acionamentoDaBombaContinua(tempoInicialBombaContinua*pow(10,3));
    }else{
        capituraContinuaTempo = true;
    }

    if(float(umidadeDeCampo - umidadeDeCampo*.075) <  UmidadeSoloGotijamento.getUmidade() < float(umidadeDeCampo + umidadeDeCampo*.075) and numeroDeLigacoesBombaGotijamento <= 6){
        //Bloco Bomba Gotijamento
        if(capituraGotijamentoTempo) {
            tempoInicialBombaGotijamentoEmMinutos = relogio.getTime().min;
            tempoInicialBombaGotijamento = millis();
            capituraGotijamentoTempo = false;
        }else{
            if(relogio.getTime().min - tempoInicialBombaGotijamentoEmMinutos >= 30) {
                tempoInicialBombaGotijamento = millis();
                numeroDeLigacoesBombaGotijamento+=1;
            }else{
                if(relogio.getTime().min - tempoInicialBombaGotijamentoEmMinutos < 0 ){
                    tempoInicialBombaGotijamentoEmMinutos = 30 - tempoInicialBombaGotijamentoEmMinutos + (relogio.getTime().min-1);
                }
            }
            acionamentoDaBombaGotijamento(tempoInicialBombaGotijamento);
        }
    }else{
        capituraGotijamentoTempo = true;
        numeroDeLigacoesBombaGotijamento = 0;
    }

}
bool acionamentoDaBombaContinua(long int tempoInicial){
    if(micros()*pow(10,3) - tempoInicial > tempoQueAbombaFicaraLigada){
        digitalWrite(AcionamentoBombaContinua,HIGH);
        return true;
    }
    digitalWrite(AcionamentoBombaContinua,LOW);
    return false;
}
bool acionamentoDaBombaGotijamento(long int tempoInicial){
    if(micros()*pow(10,3) - tempoInicial > tempoQueAbombaFicaraLigada/6){
        digitalWrite(AcionamentoBombaGotejamento,HIGH);
        return true;
    }
    digitalWrite(AcionamentoBombaGotejamento,LOW);
    return false;
}
