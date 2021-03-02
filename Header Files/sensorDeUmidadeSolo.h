#include<Arduino.h>
#ifndef SENSORDEUMIDADESOLOH
#define SENSORDEUMIDADESOLOH
class SensorDeUmidadeSolo{
	private:
		int umidadeSoloPort;
	public:
		SensorDeUmidadeSolo(int);
		float getUmidade();
};
#endif