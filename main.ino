#include <Arduino.h>
#include <DS1307.h>
#include "./Resources Files/dht.cpp"
#include "./Resources Files/SensorPH.cpp"
#include "./Resources Files/SensorCondutividade.cpp"
#include "./Resources Files/sensorDeUmidadeSolo.cpp"
#define relogio1 2
#define relogio2 3
#define SensorPHPorte A0
#define SensorCondutividadeEletrica A1
#define SensorDeTemperaturaUmidade A2
#define AcionamentoBombaContinua 4
#define AcionamentoBombaGotejamento 5
//vazão =  dv / dt
#define volumeDeAguaDaIrrigacao 10 //10 litros
#define vazaoDaBomba 900
float tempoQueAbombaFicaraLigada = float(volumeDeAguaDaIrrigacao)/float((vazaoDaBomba/(60*60)));//Em segundos

SensorPH sensor(SensorPHPorte);
SensorCondutividade condutividadeEletrica(SensorCondutividadeEletrica);
dht DHT;
DS1307 relogio(relogio1, relogio2);

bool acionamentoDaBomba(long int);

double temperatura(){
    delay(2000);
    return DHT.temperature;
}
double umidade(){
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
    Serial.println(sensor.getValorPH());
}
bool acionamentoDaBombaContinua(long int tempoInicial){
  if(micros()*pow(10,3) - tempoInicial > tempoQueAbombaFicaraLigada){
    digitalWrite(AcionamentoBombaContinua,HIGH);
    return true;
  }
  digitalWrite(AcionamentoBombaContinua,LOW);
  return false;
}
