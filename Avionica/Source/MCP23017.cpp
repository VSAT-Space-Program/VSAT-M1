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

	Last_Port=0;

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
	Wire->requestFrom(Address, 1U);
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

	//This class uses the Byte Mode to read only the GPIOB
	Wire->beginTransmission(Address);
	Wire->write(GPIOB);
	if (Wire->endTransmission() != 0)
		return false;


	return true;
}

bool MCP23017::Read_Byte(uint8_t *Value,uint8_t Port){

	//if the last
	if(Last_Port != Port){
		Wire->beginTransmission(Address);
		Wire->write(Port);
		if (Wire->endTransmission() != 0)
			return false;

		Last_Port = Port;
	}

	uint8_t n_bytes = Wire->requestFrom(Address, 1U);

	if (n_bytes != 1)
		//TODO - Flush the twi buffer
		return false;

	*Value = Wire->read();

	return true;
}

bool MCP23017::Write_Byte(uint8_t Value,uint8_t Port){

	Wire->beginTransmission(Address);
	Wire->write(Port);
	Wire->write(Value);
	if (Wire->endTransmission() != 0)
		return false;


	return true;
}

/*
 * 1 = Pin is configured as an input.
 * 0 = Pin is configured as an output.
 */

bool MCP23017::Set_Port_Direction(uint8_t Value,uint8_t Port){

	uint8_t IODIRx=0xFF;

	if(Port==GPIOA)
		IODIRx=IODIRA;

	if(Port==GPIOB)
		IODIRx=IODIRB;

	if (IODIRx==0xFF)
		return false;

	//Read_Reg(&value,IODIRx);

	Wire->beginTransmission(Address);
	Wire->write(IODIRx);
	Wire->write(Value);
	if (Wire->endTransmission() != 0)
		return false;

	return true;
}

bool MCP23017::Read_Reg(uint8_t *Value,uint8_t Reg){


	Wire->beginTransmission(Address);
	Wire->write(Reg);
	if (Wire->endTransmission() != 0)
		return false;

	uint8_t n_bytes = Wire->requestFrom(Address, 1U);

	if (n_bytes != 1)
		//TODO - Flush the twi buffer
		return false;

	*Value =  Wire->read();

	return true;

}

/*	Set only one bit
 *  bit from 0 to 7
 */
bool MCP23017::Set_bit(uint8_t bit, uint8_t Port){

	uint8_t value;
	bool status;

	status = Read_Byte(&value,Port);

	value |= (1<<bit);

	if(status)
		status = Write_Byte(value,Port);

	return status;
}

/*	Clear only one bit
 *  bit from 0 to 7
 */
bool MCP23017::Clear_bit(uint8_t bit, uint8_t Port){

	uint8_t value;
	bool status;

	status = Read_Byte(&value,Port);

	value &= ~(1<<bit);

	if(status)
		status = Write_Byte(value,Port);

	return status;
}

