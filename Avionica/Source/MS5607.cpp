/*
  * MS5607.cpp
    *  Created on: 30 de mar�o de 2019
        Author: joaobrito
  */

#include "MS5607.h"
#include <Wire.h>

MS5607::MS5607()
{

}


uint8_t MS5607::Initialize(TwoWire* Wire){

	this->Wire = Wire;
	readCalibration();
	return 1;
}

uint8_t MS5607::resetDevice(void){
  Wire->beginTransmission(MS5607_ADDRESS);
  Wire->write(RESET);
  uint8_t error = Wire->endTransmission();
  if(error == 0){
	_delay_ms(4);
    return(1);
  }else{return(0);}
}

uint8_t MS5607::readCalibration(){

  if(resetDevice() &&
    readUInt_16(PROM_READ+2, C1) &&
    readUInt_16(PROM_READ+2*2, C2) &&
    readUInt_16(PROM_READ+2*3, C3) &&
    readUInt_16(PROM_READ+2*4, C4) &&
    readUInt_16(PROM_READ+2*5, C5) &&
    readUInt_16(PROM_READ+2*6, C6)
  ){
    #ifdef MS5607_DEBUG
    Serial.print("C1 - ");
    Serial.println(C1);
    Serial.print("C2 - ");
    Serial.println(C2);
    Serial.print("C3 - ");
    Serial.println(C3);
    Serial.print("C4 - ");
    Serial.println(C4);
    Serial.print("C5 - ");
    Serial.println(C5);
    Serial.print("C6 - ");
    Serial.println(C6);
    #endif


    const1 = ((float)C5)*((int)1<<8);
//     Serial.print("const1 - ");

    const2 = ((float)C6)/(float)((long)1<<23);
//     Serial.print("const2 - ");

    const3 = (((int64_t)C2)*((long)1<<17));
//     Serial.print("const3 - ");

    const4 = ((float)C4)/((int)1<<6);
//     Serial.print("const4 - ");

    const5 = ((float)C1)*((long)1<<16);
//     Serial.print("const5 - ");

    const6 = ((float)C3)/((int)1<<7);
//     Serial.print("const6 - ");

    return (1);
  }else{return(0);}
}

int16_t MS5607::readUInt_16(uint8_t address, uint16_t &value){
  uint8_t data[2]; //4bit
  data[0] = address;
  if (readBytes(data,2))
  {
    value = (((unsigned int)data[0]*(1<<8))|(unsigned int)data[1]);
    return(1);
  }
  value = 0;
  return(0);
}

uint8_t MS5607::readBytes(uint8_t *values, uint8_t length){
  uint8_t x;

    Wire->beginTransmission(MS5607_ADDRESS);
    Wire->write(values[0]);

  uint8_t error = Wire->endTransmission();
  if (error == 0)
  {
    Wire->requestFrom((uint8_t)MS5607_ADDRESS,length);
    while(!Wire->available()) ; // wait until bytes are ready
    for(x=0;x<length;x++)
    {
      values[x] = Wire->read();
    }
    return(1);
  }
  return(0);
}

uint8_t MS5607::startConversion(uint8_t CMD){
  Wire->beginTransmission(MS5607_ADDRESS);
  Wire->write(CMD);
  uint8_t error = Wire->endTransmission();
  if(error == 0){
    switch (OSR) {
      case 256:
    	_delay_ms(1);
        break;
      case 4096:
    	_delay_ms(10);
        break;
      default:
        break;
    }
    return(1);
  }else{return(0);}
}

uint8_t MS5607::startMeasurment(void){
  Wire->beginTransmission(MS5607_ADDRESS);
  Wire->write(R_ADC);
  uint8_t error = Wire->endTransmission();
  if(error == 0){
	_delay_ms(3);
    return 1;
  }else{return 0;}
}

uint8_t MS5607::getDigitalValue(void){

	if(!startConversion(CONV_D1))
		return 0;

	if(!startMeasurment())
		return 0;

	if(!readDigitalValue(DP))
		return 0;

	if(!startConversion(CONV_D2))
		return 0;

	if(!startMeasurment())
		return 0;

	if(!readDigitalValue(DT))
		return 0;

//	if(startConversion(CONV_D1)){
//		if(startMeasurment()){
//		if(readDigitalValue(DP));
//		}
//		}else{return 0;}
//	if(startConversion(CONV_D2)){
//	  if(startMeasurment()){
//		if(readDigitalValue(DT));
//	  }
//	}else{return 0;}

	#ifdef MS5607_DEBUG
	Serial.print("D1 - ");
	Serial.println(DP);
	Serial.print("D2 - ");
	Serial.println(DT);
	#endif

	return 1;
}

uint8_t MS5607::readDigitalValue(uint32_t &value){
	uint8_t x, length = 3;
	unsigned char data[3];

	Wire->requestFrom((uint8_t)MS5607_ADDRESS,length);

	while(!Wire->available()) ; // wait until bytes are ready

	for(x=0;x<length;x++)
	{
	  data[x] = Wire->read();
	}

	value = (uint32_t)data[0]*1<<16|(uint32_t)data[1]*1<<8|(uint32_t)data[2];

	return(1);
	}

  double MS5607::getTemperature(void){
    dT = (double)DT - const1;
    TEMP = 2000.0 + dT * const2;
    return TEMP/100 ;
  }


  double MS5607::getPressure(void){

    OFF = const3 + dT * const4;
    SENS = const5 + dT * const6;

      if (TEMP >= 2000.0) //First Order Compensation.
      {
          OFF2 = 0;
          SENS2 = 0;
      }
      if (TEMP < 2000.0) //Second Order Compensation.
      {
          OFF2 = 61*((TEMP-2000)*(TEMP-2000))/pow(2,4);
          SENS2 = 2*((TEMP-2000)*(TEMP-2000));

      }
      if (TEMP < -1500.0) //Third Order Compensation.
      {
          OFF2 += 15*((TEMP+1500)*(TEMP+1500));
          SENS2 += 8*((TEMP+1500)*(TEMP+1500));
      }

        //Calculate Pressure.
        OFF-= OFF2;
        SENS -= SENS2;

        P = (((DP*SENS)/pow(2,21)-OFF)/pow(2,15));

    return P/100;
  }
