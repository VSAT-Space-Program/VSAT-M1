/*
 * SX1276.cpp
 *
 *  Created on: 20 de jan de 2019
 *      Author: educampos
 */

#include "SX1276.h"

static void Normal() {PORTD &= ~ ((1<<M0) | (1<<M1));}
static void WakeUp() {PORTD &= ~(1<<M0);  PORTD |= (1<<M1);}
static void PowerSaving() {PORTD |= (1<<M0); PORTD &= ~(1<<M1);}
static void Sleep() {PORTD |= (1<<M0) | (1<<M1);}

SX1276::SX1276() {
	// TODO Auto-generated constructor stub

}

SX1276::~SX1276() {
	// TODO Auto-generated destructor stub
}

bool SX1276::Initialize(Usart* Serial){

	DDRD |= (1<<M0) | (1<<M1); //Set M0 and M1 port as output

	//Put the device in sleep mode to configure it.
	Sleep();

	uint8_t msg[6];

	msg[0]= 0xc0;
	msg[1]= ADDH;
	msg[2]= ADDL;

	//TODO- Finish configuration


	Normal();

	_delay_ms(1000);
	// Finishing the procedures in sleep mode
	uint16_t Counter=0;
	while((PIND & (1<<AUX))==0)
	{
		_delay_ms(1);
		Counter++;
		if (Counter>=1000)
			return false; //fail to resert the module
	}


	return true;
}

