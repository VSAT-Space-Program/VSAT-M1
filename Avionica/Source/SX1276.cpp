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

/************************************************************************
* @param
* Address -> Module Address
************************************************************************/
SX1276::SX1276(uint16_t Address) {
	// TODO Auto-generated constructor stub
	this->Address = Address;
	Serial = NULL;

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

	uint8_t msg[6];
	//Initial configuration message
	msg[0]= 0xC0;
	msg[1]= uint8_t(Address>>8); //ADDH
	msg[2]= uint8_t(Address);  //ADDL
	msg[3]= AIR_BPS_2400 | USART_BPS_9600 |P_8N1;
	msg[4]= CHAN;
	msg[5]= 0x44;

	//Send the configuration to the LoRa Module
	this->Serial->writeBytes(msg, 6);

	//Wait the message to be sent
	_delay_us(1000);

	//Ask the module to send configuration
	uint8_t command[3] = {0xc1,0xc1,0xc1};
	this->Serial->writeBytes(command, 3);

	//read the module configuration
	uint8_t output[6];
	ReadBytes(output,6);

	//Verify if the LoRa module has the right configuration
	bool aux= true;
	for(uint8_t idx=0;idx<sizeof(msg);idx++)
	{
		aux = aux & (msg[idx] == output[idx]);
	}

	if(aux==false)
	{
		//Fail to write or read the configurations
		return false;
	}

	//Wake up the module
	Normal();

	_delay_ms(1000);

	uint16_t Counter=0;
	//wait the module wake Up (Aux == 1)
	while((PIND & (1<<AUX))==0)
	{
		_delay_ms(1);
		//avoid infinite loop, the "Counter" was introduced.
		Counter++;
		if (Counter>=1000)// considering the _delay_ms, this counter will wait 1s
			return false; //fail to start the module
	}


	return true; //successful
}


bool SX1276::Send(uint16_t Adrress, uint8_t Channel, uint8_t* data, uint8_t Lenght){

	if (Serial == NULL)
		return false;

	Serial->write(Adrress);
	Serial->write(Channel);
	Serial->writeBytes(data,Lenght);

	return true;
}

bool SX1276::Received(uint8_t* data, uint8_t* Lenght){

	if (Serial == NULL)
	  	//Serial port has not been initialized
		return false;

//		char buf[MAX_TX_SIZE];
//		int cnt = 0;
//		if (AUX == 0)
//		{
//			 do {
//			 buf[cnt++] = Serial->read();
//		 } while (Serial->available() < Lenght && cnt < MAX_TX_SIZE);
//		}

//	return (buf, cnt); //successful

	//TODO - Conferir qual o nível lógico para representar o fim da comunicação
	//TODO - incluir timer para evitar loop infinito ou usar interrupção
	while(AUX_IS_HIGH);

	*Lenght = Serial->available();

	//TODO - incluir um teste para saber se a leitura foi OK
	ReadBytes(data, *Lenght);

	return true;
}


bool SX1276::ReadBytes(uint8_t* data, uint8_t Lenght){

	if (Serial == NULL)
		//Serial port has not been initialized
		return false;

	//TODO - Para evitar loop infinito. Adicionar um contador
	while(Serial->available()<Lenght);

	for(uint8_t idx=0 ; idx<Lenght ; idx++)
		data[idx]=Serial->read();

	return true;
}


bool SX1276::Default_Setup(void){

}
