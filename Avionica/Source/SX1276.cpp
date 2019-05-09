/*
 * SX1276.cpp
 *
 *  Created on: 20 de jan de 2019
 *      Authors: educampos e Saulo Aislan da Silva Eleutério
 */

#include "SX1276.h"
#include "string.h"

static void Normal() {PORTD &= ~ ((1<<M0) | (1<<M1));}
static void WakeUp() {PORTD &= ~(1<<M0);  PORTD |= (1<<M1);}
static void PowerSaving() {PORTD |= (1<<M0); PORTD &= ~(1<<M1);}
static void Sleep() {PORTD |= ((1<<M0) | (1<<M1));}

/************************************************************************
* @param
* Address -> Module Address
************************************************************************/
SX1276::SX1276() {

	this->Address = 0;
	this->Channel = 0;
	Serial = NULL;

}

SX1276::~SX1276() {

}


bool WaitAUX_H(){

  uint16_t cnt = 0;

  while(!AUX_IS_HIGH)
  {
	  _delay_ms(1);
	  //avoid infinite loop, the "Counter" was introduced.
	  cnt++;
	  if (cnt>=1000)// considering the _delay_ms, this counter will wait 1s
		  return false; //fail to AUX HIGH
  }

  return true; //successful

}

bool SX1276::Initialize(Usart* Serial,uint16_t Address, uint8_t Channel){

	//Attach the serial port
	this->Serial = Serial;
	this->Address=Address;
	this->Channel = Channel;

	//TODO - the serial speed must be 9600 during the configuration

	DDRD |= ((1<<M0) | (1<<M1)); //Set M0 and M1 port as output

	/*
	 * When the module is powered on, AUX outputs low level immediately, conducts hardware self-check and sets the operating mode on
	 * the basis of the user parameters. During the process, the AUX keeps low level. After the process completed, the AUX outputs high
	 * level and starts to work as per the operating mode combined by M1 and M0. Therefore, the user needs to wait the AUX rising edge
	 * as the starting point of module’s normal work.
	 */

	if(!WaitAUX_H())
		return false;

	//Put the device in sleep mode to configure it.
	Sleep();

	_delay_ms(10);

	uint8_t msg[6];

	//Initial configuration message
	msg[0]= 0xC0;
	msg[1]= uint8_t(Address>>8); //ADDH
	msg[2]= uint8_t(Address);  //ADDL
	msg[3]= AIR_BPS_2400 | USART_BPS_9600 |P_8N1;
	msg[4]= Channel;
	msg[5]= FIXED_MODE|PULL_UP_MODE|WAKE_UP_TIME_1250MS|TURN_FEC|POWER_30DB;

	//Send the configuration to the LoRa Module
	this->Serial->writeBytes(msg, 6);

	_delay_ms(20);

	//Ask the module to send configuration
	uint8_t command[3] = {0xc1,0xc1,0xc1};
	Serial->clear();
	this->Serial->writeBytes(command, 3);

	//read the module configuration
	uint8_t output[6]={0,0,0,0,0,0};
	if (!ReadBytes(output,6))
		return false;

	//Verify if the LoRa module has the right configuration
	if (memcmp(msg, output, 6)!=0)
		return false;//Fail to write or read the configurations

	//Wake up the module
	Normal();

	_delay_ms(10);

	if(!WaitAUX_H())
		return false;

	return true; //successful
}


bool SX1276::Send(uint16_t Adrress, uint8_t Channel, uint8_t* data, uint8_t Lenght){

	if (Serial == NULL)
		return false;

	Serial->writeByte(Adrress>>8);
	Serial->writeByte(Adrress);
	Serial->writeByte(Channel);
	Serial->writeBytes(data,Lenght);

	return true;
}

bool SX1276::Received(uint8_t* data, uint8_t* Lenght){

	if (Serial == NULL)
	  	//Serial port has not been initialized
		return false;

	//Aguarda 1 segundo para receber todos os bytes
	WaitAUX_H();

	*Lenght = Serial->available();

	if(!ReadBytes(data, *Lenght))
		return false;

	return true;
}


bool SX1276::ReadBytes(uint8_t* data, uint8_t Lenght){

	if (Serial == NULL)
		//Serial port has not been initialized
		return false;

	uint16_t cnt = 0;

	while(Serial->available()<Lenght){
		_delay_ms(1);
		//to avoid infinite loop, the "Counter" was introduced.
		cnt++;
		if (cnt>=3000)// considering the _delay_ms, this counter will wait 3s
		  return false; //fail to Serial available
	}

	for(uint8_t idx=0 ; idx<Lenght ; idx++)
		data[idx]=Serial->read();

	return true;
}


bool SX1276::Default_Setup(void){

}
