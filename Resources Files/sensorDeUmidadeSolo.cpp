#include "../Header Files/sensorDeUmidadeSolo.h"
SensorDeUmidadeSolo::SensorDeUmidadeSolo(int umidadeSoloPort){
	this->umidadeSoloPort = umidadeSoloPort;
	pinMode(this->umidadeSoloPort,INPUT);
}
float SensorDeUmidadeSolo::getUmidade(){
	return float(analogRead(this->umidadeSoloPort));
}