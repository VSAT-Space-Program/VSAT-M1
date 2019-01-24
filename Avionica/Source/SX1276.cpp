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

	//Attach the serial port
	this->Serial = Serial;

	DDRD |= (1<<M0) | (1<<M1); //Set M0 and M1 port as output

	//Put the device in sleep mode to configure it.
	Sleep();

	//Initial configuration message
	uint8_t msg[6];
	//Default configuration
	msg[0]= 0xc0;
	msg[1]= ADDH;
	msg[2]= ADDL;
	msg[3]= AIR_BPS_2400 | USART_BPS_9600 |P_8N1;
	msg[4]= CHAN;
	msg[5]= 0b01000100;

	uint8_t command[3] = {0xc1,0xc1,0xc1};
	this->Serial->writeBytes(command, 3);

	uint8_t output[6];
	//TODO - a porta serial não possui uma função para fazer a leitura de n bytes,
	//this->Serial->readBytes(output,6);

	//Verify if the LoRa module has the right configuration
	bool aux= true;
	for(uint8_t idx=0;idx<sizeof(msg);idx++)
	{
		aux = aux & (msg[idx] == output[idx]);
	}

	if(aux==false)
	{
		//There is something different in the configuration
		//TODO - Send the right configuration
	}


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
			return false; //fail to start the module
	}


	return true; //successful
}

bool SX1276::Send(uint16_t Adrress, uint8_t Channel, uint8_t* data, uint8_t Lenght){



}

