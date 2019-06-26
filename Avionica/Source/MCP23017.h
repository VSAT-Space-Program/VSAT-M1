/*
 * MCP23017.h
 *
 *  Created on: 19 de mar de 2019
 *      Author: Eduardo Lacerda Campos
 */

#ifndef AVIONICA_SOURCE_MCP23017_H_
#define AVIONICA_SOURCE_MCP23017_H_

#include <Wire.h>
#include "stdlib.h"
#include <inttypes.h>

#define ADDRESS_MASK 0x20

//REGISTER (Using bank=1)
//When a bit is set, the corresponding pin becomes an
//input. When a bit is clear, the corresponding pin
//becomes an output.
#define IODIRA		0x00	//DIRECTION REGISTER
#define IODIRB		0X10	//DIRECTION REGISTER
#define IPOLA		0x01	//INPUT POLARITY REGISTER
#define IPOLB		0X11	//INPUT POLARITY REGISTER
#define GPINTENA	0x02	// INTERRUPT-ON-CHANGE PINS
#define GPINTENB	0X12	// INTERRUPT-ON-CHANGE PINS
#define DEFVALA		0x03	//DEFAULT COMPARE REGISTER FOR INTERRUPT-ON-CHANGE
#define DEFVALB		0X13	//DEFAULT COMPARE REGISTER FOR INTERRUPT-ON-CHANGE
#define INTCONA		0x04	// INTERRUPT-ON-CHANGE CONTROL
#define INTCONB		0X14	// INTERRUPT-ON-CHANGE CONTROL
#define IOCON		0x05
//#define IOCON		0X15
#define GPPUA		0x06	//GPIO PULL-UP RESISTOR
#define GPPUB		0X16	//GPIO PULL-UP RESISTOR
#define INTFA		0x07
#define INTFB		0X17
#define INTCAPA		0x08
#define INTCAPB		0X18
#define GPIOA		0x09
#define GPIOB		0X19
#define OLATA		0x0A	//OUTPUT LATCH
#define OLATB		0X1A	//OUTPUT LATCH

//CONFIGURATION REGISTER
//IOCON

#define IOCON_BANK		0x80	//Controls how the registers are addressed
#define IOCON_MIRROR 	0x40	//INT Pins Mirror bit
#define IOCON_SEQOP		0x20	//Sequential Operation mode bit
#define IOCON_DISSLW	0x10	//Slew Rate control bit for SDA output
#define IOCON_HAEN		0x08 	//Hardware Address Enable bit
#define IOCON_ODR		0x04	//Configures the INT pin as an open-drain output
#define IOCON_INTPOL 	0x02 	//This bit sets the polarity of the INT output pin



class MCP23017 {
public:
	MCP23017(uint8_t Addr);
	virtual ~MCP23017();
	bool Initialize(TwoWire* Wire);
	bool Read_Byte(uint8_t *Value, uint8_t Port);
	bool Write_Byte(uint8_t Value, uint8_t Port);
	bool Set_Port_Direction(uint8_t Value,uint8_t Port);
	bool Read_Reg(uint8_t *Value, uint8_t Reg);
	bool Set_bit(uint8_t bit, uint8_t Port);
	bool Clear_bit(uint8_t bit, uint8_t Port);
private:
	TwoWire* Wire;
	uint8_t Address;
	uint8_t Last_Port;
};

#endif /* AVIONICA_SOURCE_MCP23017_H_ */
