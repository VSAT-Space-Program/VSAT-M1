/*
	* MS5607.h
		*  Created on: 30 de março de 2019
	      Author: joaobrito
	*/

#include <MS5607.h>
#include <Wire.h>

MS5607::MS5607()
{
    this->Wire = NULL;
}

MS5607::~MS5607()
{

}

bool MS5607::begin(void){
  Wire.begin();
  return(readCalibration());
}

bool MS5607::initialize(TwoWire* Wire)
{
    this->Wire = Wire;
    return true;
}

uint8_t MS5607::resetDevice(void){
  Wire->beginTransmission(MS5607_ADDRESS);
  Wire->write(RESET);
  uint8_t error = Wire.endTransmission();
  if(error == 0){
    delay(4);
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
    // Serial.print("const1 - ");
    // Serial.println(const1);
    const2 = ((float)C6)/(float)((long)1<<23);
    // Serial.print("const2 - ");
    // Serial.println(const2,4);
    const3 = (((int64_t)C2)*((long)1<<17));
    // Serial.print("const3 - ");
    // Serial.println(ToString(const3));
    const4 = ((float)C4)/((int)1<<6);
    // Serial.print("const4 - ");
    // Serial.println(const4);
    const5 = ((float)C1)*((long)1<<16);
    // Serial.print("const5 - ");
    // Serial.println(const5);
    const6 = ((float)C3)/((int)1<<7);
    // Serial.print("const6 - ");
    // Serial.println(const6);
    return (1);
  }else{return(0);}
}

uint8_t MS5607::readUInt_16(uint8_t address, unsigned int &value){
  unsigned uint8_t data[2];	//4bit
	data[0] = address;
	if (readBytes(data,2))
	{
		value = (((unsigned int)data[0]*(1<<8))|(unsigned int)data[1]);
		return(1);
	}
	value = 0;
	return(0);
}

uint8_t MS5607::readBytes(unsigned char *values, uint8_t length){
	uint8_t x;

    Wire->beginTransmission(MS5607_ADDRESS);
    Wire->write(values[0]);

	uint8_t error = Wire->endTransmission();
	if (error == 0)
	{
		Wire->requestFrom(MS5607_ADDRESS,length);
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
        delay(1);
        break;
      case 4096:
        delay(10);
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
    delay(3);
    return(1);
  }else{return(0);}
}

uint8_t MS5607::getDigitalValue(void){
    if(startConversion(CONV_D1)){
      if(startMeasurment()){
        if(readDigitalValue(DP));
      }
      }else{return 0;}
    if(startConversion(CONV_D2)){
      if(startMeasurment()){
        if(readDigitalValue(DT));
      }
    }else{return 0;}
    #ifdef MS5607_DEBUG
    Serial.print("D1 - ");
    Serial.println(DP);
    Serial.print("D2 - ");
    Serial.println(DT);
    #endif
    return 1;
}

uint8_t MS5607::readDigitalValue(unsigned long &value){
  uint8_t x, length = 3;
  unsigned uint8_t data[3];
    Wire->requestFrom(MS5607_ADDRESS,length);
    while(!Wire->available()) ; // wait until bytes are ready
    for(x=0;x<length;x++)
    {
      data[x] = Wire.read();
    }
    value = (unsigned long)data[0]*1<<16|(unsigned long)data[1]*1<<8|(unsigned long)data[2];
    return(1);
  }

  double MS5607::getTemperature(void){
    dT = (double)DT - const1;
    TEMP = 2000.0 + dT * const2;
    return TEMP/100 ;
  }

  float MS5607::getPressure(void){
    #ifdef MS5607_DEBUG
    Serial.print("D2 - ");
    Serial.println(float(DT));
    #endif
    dT = (float)DT - const1;
    #ifdef MS5607_DEBUG
    Serial.print("dT - ");
    Serial.println(dT);
    #endif

    TEMP = 2000.0 + dT * const2;
    #ifdef MS5607_DEBUG
    Serial.print("TEMP - ");
    Serial.println(TEMP/100);
    #endif

    OFF = const3 + dT * const4;
    #ifdef MS5607_DEBUG
    Serial.print("OFF - ");
    Serial.println(ToString(OFF));
    #endif

    SENS = const5 + dT * const6;
    #ifdef MS5607_DEBUG
    Serial.print("SENS - ");
    Serial.println(ToString(SENS));
    #endif

    float pa = (float)((float)DP/((long)1<<15));
    #ifdef MS5607_DEBUG
    Serial.print("PA - ");
    Serial.println(pa,2);
    #endif
    float pb = (float)(SENS/((float)((long)1<<21)));
    #ifdef MS5607_DEBUG
    Serial.print("PB - ");
    Serial.println(pb,2);
    #endif
    float pc = pa*pb;
    #ifdef MS5607_DEBUG
    Serial.print("PC - ");
    Serial.println(pc,2);
    #endif

    float pd = (float)(OFF/((float)((long)1<<15)));
    #ifdef MS5607_DEBUG
    Serial.print("PD - ");
    Serial.println(pd,2);
    #endif
    P = pc - pd;
    //P = (DP*SENS/((long)(1<<21)) - OFF)/((long)1<<15);
    #ifdef MS5607_DEBUG
    Serial.print("P - ");
    Serial.println(P);
    #endif

    return P/100;
  }

  String MS5607::ToString(uint64_t x){
     boolean flag = false; // For preventing string return like this 0000123, with a lot of zeros in front.
     String str = "";      // Start with an empty string.
     uint64_t y = 10000000000000000000;
     int res;
     if (x == 0)  // if x = 0 and this is not testet, then function return a empty string.
     {
           str = "0";
           return str;  // or return "0";
     }
     while (y > 0)
     {
            res = (int)(x / y);
            if (res > 0)  // Wait for res > 0, then start adding to string.
                flag = true;
            if (flag == true)
                str = str + String(res);
            x = x - (y * (uint64_t)res);  // Subtract res times * y from x
            y = y / 10;                   // Reducer y with 10
     }
     return str;
}
