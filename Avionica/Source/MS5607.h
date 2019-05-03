	/*
	* MS5607.h
		*  Created on: 30 de março de 2019
	      Author: joaobrito
	*/

#define MS5607_H

#include<Wire.h>

#define MS5607_ADDRESS 0X76
#define PROM_READ  0xA0
#define RESET  0x1E
#define OSR 4096
//#define CONV_D1  0x40 //OSR = 256 //calculation time = 0.60 ms
#define CONV_D1 0x48 //OSR = 4096 //calculation time = 9.04 ms
//#define CONV_D2  0x50 //OSR = 256 //calculation time = 0.60 ms
#define CONV_D2 0x58 //OSR=4096 //calculation time = 9.04 ms
#define R_ADC  0X00 //adc read


class MS5607
{
  public:
    public:
    MS5607();
    uint8_t begin();
    double getTemperature(void);
    double getPressure(void);
    uint8_t getDigitalValue(void);

  private:
    unsigned int C1,C2,C3,C4,C5,C6;
    unsigned long DP, DT;
    double dT, TEMP, P, T2;
    double const1, const2, const4, const5, const6;
    int64_t const3;
    int64_t OFF, SENS, OFF2, SENS2;

    uint8_t resetDevice(void);
    uint8_t readCalibration();
    int16_t readUInt_16(uint8_t address, unsigned int &value);
    uint8_t readBytes(unsigned char *values, uint8_t length);
    uint8_t startConversion(uint8_t CMD);
    uint8_t startMeasurment(void);
    uint8_t readDigitalValue(unsigned long &value);
    String ToString(uint64_t x);

};
