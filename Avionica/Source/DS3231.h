/*
 * DS3221.h
 *
 *  Created on: 18 de jan de 2019
 *      Author: educampos
 */

#ifndef DS3231_H_
#define DS3231_H_

#include <Wire.h>

#include "DateTime.h"

#define DS3231_ADDRESS  							0x68
#define DS3231_CONTROL  							0x0E
#define DS3231_STATUSREG 							0x0F
#define DS3231_SECOND_REGISTER    					0x00
#define DS3231_TEMPERATURE_REGISTER_LOW_BYTE    	0x12
#define DS3231_TEMPERATURE_REGISTER_HIGH_BYTE    	0x11

#define CONV_TEMPERATURE_FORCE_BIT 			0x20
#define CONV_TEMPERATURE_FORCE_BUSY_BIT 	0x08

class DS3231 {
public:
	DS3231();
	virtual ~DS3231();
	bool Initialize(TwoWire* Wire);
	bool Read();
	bool Read_Temperature();
	double Get_Temperature();
	bool Force_Temperature_Update();
	bool Adjust_Time(const DateTime& dt);

	uint8_t data[6];
	uint8_t &ss=data[0];
	uint8_t &mm=data[1];
	uint8_t &hh=data[2];
	uint8_t &y=data[3];
	uint8_t &d=data[4];
	uint8_t &m=data[5];

	uint8_t Temp;
	uint8_t Temp_float;

private:
	TwoWire* Wire;
};


#endif /* DS3231_H_ */
