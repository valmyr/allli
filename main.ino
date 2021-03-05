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
#define relogioSCL 2
#define relogioSDA 3
#define IntervaloGotejamento 30 // 30 mim
#define AcionamentoBombaContinua 4
#define AcionamentoBombaGotejamento 5
#define umidadeDeCampo 500 //ideal
#define volumeDeAguaDaIrrigacao 6 //100 litros
#define vazaoDaBomba 900 // As bombas usadas tem uma vazão de 900 / h

//vazão =  dv / dt
float tempoQueAbombaFicaraLigada = float(volumeDeAguaDaIrrigacao)/float((float(vazaoDaBomba)/float(60*60)));//Em segundos
int n = 0;
long int tempoInicialBombaContinua = 0;bool capituraContinuaTempo = true;
long int tempoInicialBombaGotejamentoEmMinutos = 0,tempoInicialBombaGotejamento = 0;bool capituraGotejamentoTempo = true;
int numeroDeLigacoesBombaGotejamento = 8;
SensorPH sensor(SensorPHPorte);
SensorCondutividade condutividadeEletrica(SensorCondutividadeEletrica);
SensorDeUmidadeSolo UmidadeSoloContinua(SensorUmidadeDoSoloContinua);
SensorDeUmidadeSolo UmidadeSoloGotejamento(SensorUmidadeDoSoloGotejamento);

dht DHT;
DS1307 relogio(relogioSCL, relogioSDA);

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
    Serial.begin(19200);
    relogio.setDOW(5);
    relogio.setDate(2,3,2021);
    relogio.setTime(2,55,58);
    relogio.setSQWRate(SQW_RATE_1);
    relogio.halt(false);
    relogio.enableSQW(true);
    pinMode(AcionamentoBombaContinua,OUTPUT);
    pinMode(AcionamentoBombaGotejamento,OUTPUT);
    digitalWrite(AcionamentoBombaContinua,LOW);
    digitalWrite(AcionamentoBombaGotejamento,LOW);
}
void loop() {
    DHT.read11(SensorDeTemperaturaUmidade);
    //Serial.println(relogio.getTimeStr(2));
    //********************************************************//
    if(capituraContinuaTempo and  UmidadeSoloContinua.getUmidade()>= float(umidadeDeCampo - umidadeDeCampo*.075)  and UmidadeSoloContinua.getUmidade()  < float(umidadeDeCampo + umidadeDeCampo*.075)){
        //Bloco Bomba Continua
        tempoInicialBombaContinua = millis();
        capituraContinuaTempo = false;
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
    if(capituraGotejamentoTempo and UmidadeSoloGotejamento.getUmidade()>= float(umidadeDeCampo - umidadeDeCampo*.075)  and UmidadeSoloGotejamento.getUmidade()  < float(umidadeDeCampo + umidadeDeCampo*.075)){
        tempoInicialBombaGotejamentoEmMinutos = relogio.getTime().sec+IntervaloGotejamento+float(tempoQueAbombaFicaraLigada)/6.0;//quando for usar minutos lembre-se de converter essa linha
        if(59 < tempoInicialBombaGotejamentoEmMinutos and  relogio.getTime().sec == 59)
            tempoInicialBombaGotejamentoEmMinutos-=59;
        capituraGotejamentoTempo = false;
        tempoInicialBombaGotejamento = millis();
        acionamentoDaBombaGotejamento(tempoInicialBombaGotejamento*pow(10,-3));
        numeroDeLigacoesBombaGotejamento = 1 ;
    }else{
        capituraGotejamentoTempo = true;
    }
    if(digitalRead(AcionamentoBombaGotejamento)){
        Serial.println("Bomba Acionada Gotejamento");
        acionamentoDaBombaGotejamento(tempoInicialBombaGotejamento*pow(10,-3));
    }else{
        Serial.println("Bomba parada Gotejamento");
    }
    if(!digitalRead(AcionamentoBombaGotejamento) and relogio.getTime().sec >= tempoInicialBombaGotejamentoEmMinutos and numeroDeLigacoesBombaGotejamento < 6) {
        tempoInicialBombaGotejamentoEmMinutos = relogio.getTime().sec+IntervaloGotejamento+float(tempoQueAbombaFicaraLigada)/6.0;
        if(tempoInicialBombaGotejamentoEmMinutos > 59 and relogio.getTime().sec == 59)
            tempoInicialBombaGotejamentoEmMinutos-=59;
        ++numeroDeLigacoesBombaGotejamento;
        tempoInicialBombaGotejamento = millis();\
        acionamentoDaBombaGotejamento(tempoInicialBombaGotejamento * pow(10, -3));
    }
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
    if(millis()*pow(10,-3) < float(tempoQueAbombaFicaraLigada)/6.0 + float(tempoInicial)){
        digitalWrite(AcionamentoBombaGotejamento,HIGH);
        return true;
    }
    digitalWrite(AcionamentoBombaGotejamento,LOW);
    return false;
}
