//apenas uma tentativa falha!

#include <Arduino.h>
#include <DS1307.h>
#include<LiquidCrystal.h>
#include "Resources Files/dht.cpp"
#include "Resources Files/SensorPH.cpp"
#include "Resources Files/SensorCondutividade.cpp"
#include "Resources Files/sensorDeUmidadeSolo.cpp"
#define SensorPHPorte A0
#define SensorCondutividadeEletrica A1
#define SensorDeTemperaturaUmidade A2
#define SensorUmidadeDoSoloContinua A3
#define SensorUmidadeDoSoloGotejamento A4
#define relogioSDA 2
#define relogioSCL 3
#define IntervaloGotejamento 3 // Intervalo da Bomba de gotejamento 30 mim. Atualizar quando o circuito estiver pronto.
//Portas dos relés para ligar as respectivas bombas.
#define AcionamentoBombaContinua 4
#define AcionamentoBombaGotejamento 5
#define umidadeDeCampo 500 //idealizado por mim para efeitos de teste de lógica. Atulizar quando o circuito estiver pronto.
#define volumeDeAguaDaIrrigacao 3 //volume em litros. Atualizar quando o circuito estiver pronto.
#define vazaoDaBomba 900 // As bombas usadas terão vazão de 900 / h
//LCD Ports
#define PortRS 13
#define PortE 12
#define PortD3 11
#define PortD4 10
#define PortD5 9
#define PortD6 8

//vazão =  dv / dt
float tempoQueAbombaFicaraLigada = float(volumeDeAguaDaIrrigacao)/float((float(vazaoDaBomba)/float(60*60)));//Em segundos
int n = 0;
long int tempoInicialBombaContinua = 0;bool capituraContinuaTempo = true;
long int tempoInicialBombaGotejamentoEmMinutos = 0,tempoInicialBombaGotejamento = 0;bool capituraGotejamentoTempo = true;
int numeroDeLigacoesBombaGotejamento = 8;

//Implementar o botão de controle para mostrar esses dados no display.
SensorPH sensor(SensorPHPorte);
SensorCondutividade condutividadeEletrica(SensorCondutividadeEletrica);
SensorDeUmidadeSolo UmidadeSoloContinua(SensorUmidadeDoSoloContinua);
SensorDeUmidadeSolo UmidadeSoloGotejamento(SensorUmidadeDoSoloGotejamento);
//Sensor De umidade relativa do Ar e Temperatura.
dht DHT;
//Relogio
DS1307 relogio(relogioSCL, relogioSDA);
//LCD
LiquidCrystal lcd(PortRS,PortE,PortD3,PortD4,PortD5,PortD6);

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
    lcd.begin(16,4);
    Serial.begin(9600);
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
    lcd.setCursor(0,0);
    lcd.print(relogio.getTime().hour);
    lcd.print(":");
    lcd.print(relogio.getTime().min);
    lcd.setCursor(8,0);
    lcd.print(relogio.getTime().dow);
    lcd.print("/");
    lcd.print(relogio.getTime().mon);
    lcd.print("/");
    lcd.print(relogio.getTime().year);
    //********************************************************//
    if(capituraContinuaTempo and  UmidadeSoloContinua.getUmidade()>= float(umidadeDeCampo - umidadeDeCampo*.075)  and UmidadeSoloContinua.getUmidade()  < float(umidadeDeCampo + umidadeDeCampo*.075)){
        //Bloco Bomba Continua
        tempoInicialBombaContinua = millis()* pow(10, -3);
        capituraContinuaTempo = false;
        acionamentoDaBombaContinua(tempoInicialBombaContinua );
    }else{
        capituraContinuaTempo = true;
    }
    lcd.setCursor(0,1);
    if(digitalRead(AcionamentoBombaContinua)){
        Serial.println("Bomba Acionada Continua");
        lcd.print("B1 ON\n");
        acionamentoDaBombaContinua(tempoInicialBombaContinua );
    }else{
        lcd.print("B1 OFF\n");
        Serial.println("Bomba parada Continua");
    }
    //**************************************************************//
    if(capituraGotejamentoTempo and UmidadeSoloGotejamento.getUmidade()>= float(umidadeDeCampo - umidadeDeCampo*.075)  and UmidadeSoloGotejamento.getUmidade()  < float(umidadeDeCampo + umidadeDeCampo*.075)){
        tempoInicialBombaGotejamentoEmMinutos = relogio.getTime().sec+IntervaloGotejamento+float(tempoQueAbombaFicaraLigada)/6.0;//quando for usar minutos lembre-se de converter essa linha
        if(59 <= tempoInicialBombaGotejamentoEmMinutos and  relogio.getTime().sec == 59)
            tempoInicialBombaGotejamentoEmMinutos-=59;
        capituraGotejamentoTempo = false;
        tempoInicialBombaGotejamento = millis()* pow(10, -3);
        acionamentoDaBombaGotejamento(tempoInicialBombaGotejamento);
        numeroDeLigacoesBombaGotejamento = 1 ;
    }else{
        capituraGotejamentoTempo = true;
    }
    lcd.setCursor(0,2);
    if(digitalRead(AcionamentoBombaGotejamento)){
        Serial.println("Bomba Acionada Gotejamento");
        lcd.print("B2 ON\n");
        acionamentoDaBombaGotejamento(tempoInicialBombaGotejamento );
    }else{
        Serial.println("Bomba parada Gotejamento");
        lcd.print("B2 OFF\n");
    }
    if(!digitalRead(AcionamentoBombaGotejamento) and relogio.getTime().sec >= tempoInicialBombaGotejamentoEmMinutos and numeroDeLigacoesBombaGotejamento < 6) {
        tempoInicialBombaGotejamentoEmMinutos = relogio.getTime().sec+IntervaloGotejamento+float(tempoQueAbombaFicaraLigada)/6.0;
        if(tempoInicialBombaGotejamentoEmMinutos >= 59 and relogio.getTime().sec == 59)
            tempoInicialBombaGotejamentoEmMinutos-=59;
        ++numeroDeLigacoesBombaGotejamento;
        tempoInicialBombaGotejamento = millis()* pow(10, -3);
        acionamentoDaBombaGotejamento(tempoInicialBombaGotejamento );
    }
    Serial.println(tempoInicialBombaGotejamentoEmMinutos);
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
