/*
 * MCP23017.cpp
 *
 *  Created on: 19 de mar de 2019
 *      Author: Eduardo Lacerda Campos
 */

#include "MCP23017.h"

MCP23017::MCP23017(uint8_t Addr) {

	Address = ADDRESS_MASK | Addr;

	Wire = NULL;

}

MCP23017::~MCP23017() {
	// TODO Auto-generated destructor stub
}

/***********************************************************************
Initialize the MCP23017 registers
Input:  TWI object pointer
Output: false -> Failure
	    true  -> Success
************************************************************************/

bool MCP23017::Initialize(TwoWire* Wire){

	this->Wire = Wire;

	// procedure to discove if bank=1 or 0
	Wire->beginTransmission(Address);
	Wire->write(0x01);
	Wire->endTransmission();
	Wire->requestFrom(Address, 1);
	uint8_t Value = Wire->read();

	if (Value== 0xFF) // bank = 0
	{

		// IOCON.BANK=0 then must be changed to 1
		Wire->beginTransmission(Address);
		Wire->write(0x0A); // IOCON address when bank=1
		Wire->write(IOCON_BANK);
		if (Wire->endTransmission() !=0)
				return false;

	}

	//Byte mode (Sequential operation disabled)
	Wire->beginTransmission(Address);
	Wire->write(IOCON);
	Wire->write(IOCON_BANK | IOCON_SEQOP);
	if (Wire->endTransmission() != 0)
		return false;

	//This class use the Byte Mode to read only the GPIOB
	Wire->beginTransmission(Address);
	Wire->write(GPIOB);
	if (Wire->endTransmission() != 0)
		return false;


	return true;
}

bool MCP23017::Read_Byte(uint8_t *Value){


//	Wire->beginTransmission(Address);
//	Wire->write(GPIOB);
//	if (Wire->endTransmission() != 0)
//		return false;
//
	uint8_t n_bytes = Wire->requestFrom(Address, 1);

	if (n_bytes != 1)
		//TODO - Flush the twi buffer
		return false;

	*Value = Wire->read();

	return true;
}
