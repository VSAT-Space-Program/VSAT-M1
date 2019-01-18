/*
 * DS3221.h
 *
 *  Created on: 18 de jan de 2019
 *      Author: educampos
 */

#ifndef DS3221_H_
#define DS3221_H_

#include <Wire.h>

#define DS3231_ADDRESS  0x68
#define DS3231_CONTROL  0x0E
#define DS3231_STATUSREG 0x0F

class DS3221 {
public:
	DS3221();
	virtual ~DS3221();
	bool Initialize(TwoWire* Wire);
	bool Read();

	uint8_t data[6];
	uint8_t &ss=data[0];
	uint8_t &mm=data[1];
	uint8_t &hh=data[2];
	uint8_t &y=data[3];
	uint8_t &d=data[4];
	uint8_t &m=data[5];

private:
	TwoWire* Wire;
};

#endif /* DS3221_H_ */
