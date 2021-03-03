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
#define volumeDeAguaDaIrrigacao 20 //100 litros
#define vazaoDaBomba 900 // As bombas usadas tem uma vazão de 900 / h

//vazão =  dv / dt
float tempoQueAbombaFicaraLigada = float(volumeDeAguaDaIrrigacao)/float((float(vazaoDaBomba)/float(60*60)));//Em segundos

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
    
    Serial.println("Continua");
    Serial.println( UmidadeSoloContinua.getUmidade());
    Serial.println('\n');
    Serial.println( tempoQueAbombaFicaraLigada);
    Serial.println(digitalRead(AcionamentoBombaContinua));
    delay(500);
   Serial.println("SoloGotejamento");
   Serial.println(UmidadeSoloGotejamento.getUmidade());
   Serial.println(digitalRead(AcionamentoBombaGotejamento));
   Serial.println('\n');
    if( UmidadeSoloContinua.getUmidade()>= float(umidadeDeCampo - umidadeDeCampo*.075)  and UmidadeSoloContinua.getUmidade()  < float(umidadeDeCampo + umidadeDeCampo*.075)){
    //if(UmidadeSoloContinua.getUmidade() == 0){
   
        //Bloco Bomba Continua
        if(capituraContinuaTempo) {
            tempoInicialBombaContinua = millis();
            capituraContinuaTempo = false;
        }else{
            if(digitalRead(AcionamentoBombaContinua))
                tempoInicialBombaContinua = millis();
            }
            acionamentoDaBombaContinua(tempoInicialBombaContinua*pow(10,-3));
    }else{
        capituraContinuaTempo = true;
    }
       if(UmidadeSoloGotejamento.getUmidade()>= float(umidadeDeCampo - umidadeDeCampo*.075)  and UmidadeSoloGotejamento.getUmidade()  < float(umidadeDeCampo + umidadeDeCampo*.075)){
        //Bloco Bomba Gotejamento
        if(capituraGotejamentoTempo) {
            tempoInicialBombaGotejamentoEmMinutos = relogio.getTime().min;
            tempoInicialBombaGotejamento = millis();
            capituraGotejamentoTempo = false;
        }else{
            if(relogio.getTime().min - tempoInicialBombaGotejamentoEmMinutos >= IntervaloGotejamento) {
                tempoInicialBombaGotejamento = millis();
                numeroDeLigacoesBombaGotejamento+=1;
            }else{
                if(relogio.getTime().min - tempoInicialBombaGotejamentoEmMinutos < 0 ){
                    tempoInicialBombaGotejamentoEmMinutos = IntervaloGotejamento - tempoInicialBombaGotejamentoEmMinutos + (relogio.getTime().min-1);
                }
            }
            acionamentoDaBombaGotejamento(tempoInicialBombaGotejamento);
        }
    }else{
        capituraGotejamentoTempo = true;
        numeroDeLigacoesBombaGotejamento = 0;
    }

}
bool acionamentoDaBombaContinua(long int tempoInicial){
    Serial.println("Bomba Acionada Continua");
    if(millis()*pow(10,-3) - tempoInicial > tempoQueAbombaFicaraLigada){
        digitalWrite(AcionamentoBombaContinua,HIGH);
        return true;
    }
    Serial.println("Bomba parada Continua");
    digitalWrite(AcionamentoBombaContinua,LOW);
    return false;
}
bool acionamentoDaBombaGotejamento(long int tempoInicial){
    Serial.println("Bomba Acionada Gotejamento");
    if(millis()*pow(10,-3) - tempoInicial > tempoQueAbombaFicaraLigada/6){
        digitalWrite(AcionamentoBombaGotejamento,HIGH);
        return true;
    }
    digitalWrite(AcionamentoBombaGotejamento,LOW);
    Serial.println("Bomba parada Gotejamento");
    return false;
}
