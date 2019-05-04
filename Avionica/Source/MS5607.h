	/*
	* MS5607.h
		*  Created on: 30 de mar�o de 2019
	      Author: joaobrito
	*/

#define MS5607_H
//#define MS5607_DEBUG
//#include <Arduino.h>
#include <Wire.h>
#include <util/delay.h>


#define MS5607_ADDRESS 0X77 // the last bit is related to the !CBS pin
#define PROM_READ  0xA0
#define RESET  0x1E
#define OSR 4096
//#define CONV_D1  0x40 //OSR = 256 //calculation time = 0.60 ms
#define CONV_D1 0x48 //OSR = 4096 //calculation time = 9.04 ms
//#define CONV_D2  0x50 //OSR = 256 //calculation time = 0.60 ms
#define CONV_D2 0x58 //OSR=4096 //calculation time = 9.04 ms
#define R_ADC  0X00 //ADC read


class MS5607
{
  public:
    public:
    MS5607();
    uint8_t Initialize(TwoWire* Wire);
    double getTemperature(void);
    double getPressure(void);
    uint8_t getDigitalValue(void);

private:
    TwoWire* Wire;

    uint16_t C1,C2,C3,C4,C5,C6;
    uint32_t DP, DT;
    double dT, TEMP, P, T2;
    double const1, const2, const4, const5, const6;
    int64_t const3;
    int64_t OFF, SENS, OFF2, SENS2;

    uint8_t resetDevice(void);
    uint8_t readCalibration();
    int16_t readUInt_16(uint8_t address, uint16_t &value);
    uint8_t readBytes(uint8_t *values, uint8_t length);
    uint8_t startConversion(uint8_t CMD);
    uint8_t startMeasurment(void);
    uint8_t readDigitalValue(uint32_t &value);
    String ToString(uint64_t x);

};
