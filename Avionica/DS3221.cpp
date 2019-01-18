/*
 * DS3221.cpp
 *
 *  Created on: 18 de jan de 2019
 *      Author: educampos
 */

#include "DS3221.h"

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }

DS3221::DS3221() {
	// TODO Auto-generated constructor stub

}

DS3221::~DS3221() {
	// TODO Auto-generated destructor stub
}

bool DS3221::Initialize(TwoWire* Wire)
{
	this->Wire = Wire;
}

bool DS3221::Read(){

	Wire->beginTransmission(DS3231_ADDRESS);
	Wire->write(0);
	Wire->endTransmission();

	Wire->requestFrom(DS3231_ADDRESS, 7);

	ss = bcd2bin(Wire->read() & 0x7F);
	mm = bcd2bin(Wire->read());
	hh = bcd2bin(Wire->read());
	Wire->read();
	d = bcd2bin(Wire->read());
	m = bcd2bin(Wire->read());
	y = bcd2bin(Wire->read());

}
