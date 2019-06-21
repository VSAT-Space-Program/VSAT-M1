/*
 * NEO6M.cpp
 *
 *  Created on: 20 de jan de 2019
 *      Author: educampos
 */

#include "NEO6M.h"



NEO6M::NEO6M() {
	// TODO Auto-generated constructor stub

}

NEO6M::~NEO6M() {
	// TODO Auto-generated destructor stub
}

void NEO6M::Initialize(Usart* Serial){

	this->Serial = Serial;

	uint8_t buffer[20];
	memcpy_P(buffer, &setRate[0], sizeof(setRate));
	UBX_Send(buffer,sizeof(setRate));

	memcpy_P(buffer, &setGSV[0], sizeof(setGSV));
	UBX_Send(buffer,sizeof(setGSV));

	memcpy_P(buffer, &setGLL[0], sizeof(setGLL));
	UBX_Send(buffer,sizeof(setGLL));

	memcpy_P(buffer, &setGSA[0], sizeof(setGSA));
	UBX_Send(buffer,sizeof(setGSA));

	memcpy_P(buffer, &setGGA[0], sizeof(setGGA));
	UBX_Send(buffer,sizeof(setGGA));

	memcpy_P(buffer, &setRMC[0], sizeof(setRMC));
	UBX_Send(buffer,sizeof(setRMC));

	memcpy_P(buffer, &setVTG[0], sizeof(setVTG));
	UBX_Send(buffer,sizeof(setVTG));

}


void NEO6M::UBX_Send(uint8_t *msg, uint8_t len){

	//The checksum algorithm used is the 8-Bit Fletcher Algorithm
	uint8_t CK_A = 0;
	uint8_t CK_B = 0;

	for(uint8_t I=2; I< len ; I++)
	{
		CK_A = CK_A + msg[I];
		CK_B = CK_B + CK_A;
	}

	Serial->writeBytes(msg,len);
	Serial->writeByte(CK_A);
	Serial->writeByte(CK_B);

}

void NEO6M::Poll()
{

	uint8_t msg[] = {
							0xB5, 0x62, //Header
							0x01, 0x02, //Class and ID
							0x00, 0x00, //Length
							};

	//garantir que nenhuma comunicação esteja acontecendo
	while(Serial->available()>0){
		Serial->clear();
		_delay_ms(10);
	}

	UBX_Send(msg,sizeof(msg));

	while(Serial->available()<34);

	uint8_t dados[34];
	Serial->readBytes(dados,34);

	assert(memcmp(dados,msg,4)==0);
}



