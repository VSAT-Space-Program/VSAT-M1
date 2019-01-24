/*
 * DS3231.cpp
 *
 *  Created on: 18 de jan de 2019
 *      Author: educampos
 */

#include "DS3231.h"

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }

DS3231::DS3231() {
	// TODO Auto-generated constructor stub

}

DS3231::~DS3231() {
	// TODO Auto-generated destructor stub
}

bool DS3231::Initialize(TwoWire* Wire)
{
	this->Wire = Wire;

	return true;
}

bool DS3231::Read(){

	Wire->beginTransmission(DS3231_ADDRESS);
	Wire->write(DS3231_SECOND_REGISTER); //registro inicial
	uint8_t ret = Wire->endTransmission();

	if (ret!=0)
		return false; //A failure has occur

	Wire->requestFrom(DS3231_ADDRESS, 7);

	ss = bcd2bin(Wire->read());
	mm = bcd2bin(Wire->read());
	hh = bcd2bin(Wire->read());
	Wire->read(); //dia da semana
	d = bcd2bin(Wire->read());
	m = bcd2bin(Wire->read()& 0x1F);
	y = bcd2bin(Wire->read());

	return true;

}


bool DS3231::Read_Temperature(){

	Wire->beginTransmission(DS3231_ADDRESS);
	Wire->write(DS3231_TEMPERATURE_REGISTER_HIGH_BYTE); //registro inicial
	Wire->endTransmission();

	Wire->requestFrom(DS3231_ADDRESS, 2); //Quantidade de Bytes

	Temp = Wire->read();
	Temp_float = Wire->read();



	return true;

}

double DS3231::Get_Temperature()
{
	return (Temp + ((Temp_float>>6) * 0.25));
}

bool DS3231::Force_Temperature_Update()
{

	Wire->beginTransmission(DS3231_ADDRESS);
	Wire->write(DS3231_CONTROL);
	Wire->write(CONV_TEMPERATURE_FORCE_BIT);
	Wire->endTransmission();

	Wire->requestFrom(DS3231_ADDRESS, 1);

	while (Wire->read() & CONV_TEMPERATURE_FORCE_BUSY_BIT);

	return true;
}

bool DS3231::Adjust_Time(const DateTime& dt) {
	Wire->beginTransmission(DS3231_ADDRESS);
	Wire->write(DS3231_SECOND_REGISTER);
	Wire->write(bin2bcd(dt.second()));
	Wire->write(bin2bcd(dt.minute()));
	Wire->write(bin2bcd(dt.hour()));
	Wire->write(bin2bcd(0));
	Wire->write(bin2bcd(dt.day()));
	Wire->write(bin2bcd(dt.month()));
	Wire->write(bin2bcd(dt.year() - 2000));
	Wire->endTransmission();

	return true;
//
//  uint8_t statreg = read_i2c_register(DS3231_ADDRESS, DS3231_STATUSREG);
//  statreg &= ~0x80; // flip OSF bit
//  write_i2c_register(DS3231_ADDRESS, DS3231_STATUSREG, statreg);
}
