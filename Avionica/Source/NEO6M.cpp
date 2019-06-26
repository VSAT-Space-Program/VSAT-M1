/*
 * NEO6M.cpp
 *
 *  Created on: 20 de jan de 2019
 *      Author: educampos
 */

#include "NEO6M.h"



NEO6M::NEO6M() {
	// TODO Auto-generated constructor stub

	Serial=NULL;

}

NEO6M::~NEO6M() {
	// TODO Auto-generated destructor stub
}

bool NEO6M::Initialize(Usart* Serial){

	this->Serial = Serial;

	bool Status=true;

	uint8_t buffer[20];
	memcpy_P(buffer, &setRate[0], sizeof(setRate));
	Status &= UBX_Send(buffer,sizeof(setRate));

	memcpy_P(buffer, &setGSV[0], sizeof(setGSV));
	Status &= UBX_Send(buffer,sizeof(setGSV));

	memcpy_P(buffer, &setGLL[0], sizeof(setGLL));
	Status &= UBX_Send(buffer,sizeof(setGLL));

	memcpy_P(buffer, &setGSA[0], sizeof(setGSA));
	Status &= UBX_Send(buffer,sizeof(setGSA));

	memcpy_P(buffer, &setGGA[0], sizeof(setGGA));
	Status &= UBX_Send(buffer,sizeof(setGGA));

	memcpy_P(buffer, &setRMC[0], sizeof(setRMC));
	Status &= UBX_Send(buffer,sizeof(setRMC));

	memcpy_P(buffer, &setVTG[0], sizeof(setVTG));
	Status &= UBX_Send(buffer,sizeof(setVTG),true);

	return Status;

}

bool NEO6M::Verify_Checksun(uint8_t *msg, uint8_t len){

	//The checksum algorithm used is the 8-Bit Fletcher Algorithm
	uint8_t CK_A = 0;
	uint8_t CK_B = 0;

	for(uint8_t I=2; I< len-2 ; I++)
	{
		CK_A = CK_A + msg[I];
		CK_B = CK_B + CK_A;
	}

	if ((msg[len-2] == CK_A) && (msg[len-1] == CK_B))
		return true;

	return false;

}


bool NEO6M::UBX_Send(uint8_t *msg, uint8_t len, bool ack){

	//The checksum algorithm used is the 8-Bit Fletcher Algorithm
	uint8_t CK_A = 0;
	uint8_t CK_B = 0;

	for(uint8_t I=2; I< len ; I++)
	{
		CK_A = CK_A + msg[I];
		CK_B = CK_B + CK_A;
	}

	//to receive the acknowledge is necessary a clean receiving buffer
	if(ack==true){
		while(Serial->available()>0){
			Serial->clear();
			_delay_ms(10);
		}
	}

	Serial->writeBytes(msg,len);
	Serial->writeByte(CK_A);
	Serial->writeByte(CK_B);

	//doesn't require ack response
	if(ack==false)
		return true;

	//waiting for the acknowledgment
	WHILE_COUNT_US(Serial->available() < 10,1000000);

	uint8_t dados[10];
	Serial->readBytes(dados,10);

	if((dados[0]!=0xB5) || (dados[1]!=0x62))
		return false;

	if((dados[3]!=0x01))
		return false;

	//verify class ID
	if((dados[6]!=msg[2]))
		return false;

	//verify msg ID
	if((dados[7]!=msg[3]))
		return false;

	return true;

}

bool NEO6M::UBX_Polling(uint8_t* UBX_msg , void* structure_data, uint8_t payload_length){


	//para garantir que nenhuma comunicação esteja acontecendo
	while(Serial->available()>0){
		Serial->clear();
		_delay_ms(10);
	}

	UBX_Send(UBX_msg,6);

	uint8_t dados[payload_length+8];
	WHILE_COUNT_US(Serial->available() < sizeof(dados),1000000);

	Serial->readBytes(dados,sizeof(dados));

	//Verifica ser o pacote recebido é o correto
	if(memcmp(dados,UBX_msg,4)!=0)
		return false;

	//Verifica o checksun
	if(!Verify_Checksun(dados,sizeof(dados)))
		return false;

	memcpy(structure_data,dados+6,payload_length);

	return true;

}


//UBX-NAV-STATUS (0x01 0x03)
const uint8_t UBX_Status[] PROGMEM= {
						0xB5, 0x62, //Header
						0x01, 0x03, //Class and ID
						0x00, 0x00, //Length
						};

const uint8_t UBX_ECEF[] PROGMEM= {
						0xB5, 0x62, //Header
						0x01, 0x06, //Class and ID
						0x00, 0x00, //Length
						};

const uint8_t UBX_NED_VEL[] PROGMEM= {
						0xB5, 0x62, //Header
						0x01, 0x12, //Class and ID
						0x00, 0x00, //Length
						};

bool NEO6M::Status_Read()
{

	STATUS_GPS *Status = new STATUS_GPS;
	uint8_t msg[sizeof(UBX_Status)];
	memcpy_P(msg, &UBX_Status[0], sizeof(UBX_Status));

	bool out = UBX_Polling(msg,Status,16);
	if(out)
		flags = Status->flags;
	else
		flags = 0;

	delete Status;

	return out;

}

bool NEO6M::ECEF_Read()
{

	ECEF_GSP *ECEF = new ECEF_GSP;

	uint8_t msg[sizeof(UBX_ECEF)];
	memcpy_P(msg, &UBX_ECEF[0], sizeof(UBX_ECEF));

	bool out = UBX_Polling(msg,ECEF,52);
	if(out){
		ECEF_X = ECEF->ECEF_X;
		ECEF_Y = ECEF->ECEF_Y;
		ECEF_Z = ECEF->ECEF_Z;
	}else{
		ECEF_X = 0;
		ECEF_Y = 0;
		ECEF_Z = 0;
	}
	delete ECEF;

	return out;


}

bool NEO6M::NED_VEL_Read()
{

	NED_VEL_GPS *Ned_V = new NED_VEL_GPS;

	uint8_t msg[sizeof(UBX_NED_VEL)];
	memcpy_P(msg, &UBX_NED_VEL[0], sizeof(UBX_NED_VEL));

	bool out =  UBX_Polling(msg,Ned_V,36);

	if(out){
		NED_Vel_N = Ned_V->velN;
		NED_Vel_E = Ned_V->velE;
		NED_Vel_D = Ned_V->velD;
	}else{
		NED_Vel_N = 0;
		NED_Vel_E = 0;
		NED_Vel_D = 0;
	}

	delete Ned_V;

	return out;

}

bool NEO6M::is_position_valid(){

	return (flags & 0x01);

}


