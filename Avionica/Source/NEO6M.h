/*
 * NEO6M.h
 *
 *  Created on: 20 de jan de 2019
 *      Author: educampos
 *      https://www.u-blox.com/en/product/ubx-m8030-series#tab-document-resources
 */

#ifndef AVIONICA_SOURCE_NEO6M_H_
#define AVIONICA_SOURCE_NEO6M_H_

#include "Usart.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

//Configura a taxa de atualização do GPS
const uint8_t setRate[] PROGMEM= {
						0xB5, 0x62, //Header
						0x06, 0x08, //Class and ID
						0x06, 0x00, //Length
						0xE8, 0x03, 0x01, 0x00, 0x01, 0x00 //Payload
						};

//Disable GSV -> GNSS Satellites in View
const uint8_t setGSV[] PROGMEM= {
						0xB5, 0x62, //Header
						0x06, 0x01, //Class and ID
						0x08, 0x00, //Length
						0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x00//Payload
						};

//Disable GLL -> Latitude and longitude, with time of position fix and status
const uint8_t setGLL[] PROGMEM= {
							0xB5, 0x62, //Header
							0x06, 0x01, //Class and ID
							0x08, 0x00, //Length
							0xF0,0x01,0x00,0x00,0x00,0x00,0x00,0x00//Payload
							};
//Disable GSA -> GNSS DOP and Active Satellites
const uint8_t setGSA[] PROGMEM= {
						0xB5, 0x62, //Header
						0x06, 0x01, //Class and ID
						0x08, 0x00, //Length
						0xF0, 0x02,0x00,0x00,0x00,0x00,0x00,0x00//Payload
						};

//Disable GGA -> Global positioning system fix data
const uint8_t setGGA[] PROGMEM= {
						0xB5, 0x62, //Header
						0x06, 0x01, //Class and ID
						0x08, 0x00, //Length
						0xF0, 0x00,0x00,0x00,0x00,0x00,0x00,0x00//Payload
						};
//Disable RMC -> Recommended Minimum data
const uint8_t setRMC[] PROGMEM= {
						0xB5, 0x62, //Header
						0x06, 0x01, //Class and ID
						0x08, 0x00, //Length
						0xF0, 0x04,0x00,0x00,0x00,0x00,0x00,0x00//Payload
						};

//Disable VTG -> Course over ground and Ground speed
const uint8_t setVTG[] PROGMEM= {
						0xB5, 0x62, //Header
						0x06, 0x01, //Class and ID
						0x08, 0x00, //Length
						0xF0, 0x05,0x00,0x00,0x00,0x00,0x00,0x00//Payload
						};

struct STATUS_GPS{
	uint32_t 	iTWO;	//GPS time of week
	uint8_t		gpsFix;	//GPSfix Type
	uint8_t		flags;	//Navigation Status Flags
	uint8_t		fixStat;//Fix Status Information
	uint8_t		flags2;
	uint32_t	ttff; 	//Time to first fix (millisecond time tag)
	uint32_t	msss; 	//Milliseconds since Startup / Reset
}__attribute__((packed));

struct ECEF_GSP{
	uint32_t 	iTWO;	//GPS time of week
	int32_t 	fTWO;	//fractional GPS time of week
	int16_t		week;
	uint8_t		gpsFix;	//GPSfix Type
	uint8_t		flags;	//Navigation Status Flags
	int32_t		ECEF_X; //cm
	int32_t		ECEF_Y;
	int32_t		ECEF_Z;
	uint32_t	pAcc; //3D Position Accuracy Estimate
	int32_t		ECEF_VX; //cm/s
	int32_t		ECEF_VY;
	int32_t		ECEF_VZ;
	uint32_t	sAcc; //Speed Accuracy Estimate
	uint16_t	pDOP; //Position DOP
	uint8_t		reserved1;
	uint8_t		numSV; //Number of SVs used in Nav Solution
	uint32_t	reserved2;
}__attribute__((packed));

struct NED_VEL_GPS{
	uint32_t 	iTWO;	//GPS time of week
	int32_t		velN;
	int32_t		velE;
	int32_t		velD;
	uint32_t	Speed;
	uint32_t	gSpeed;
	int32_t		heading; //Heading of motion 2-D
	uint32_t	sAcc; //Speed accuracy Estimate
	uint32_t	cAcc; //Course / Heading accuracy estimate
}__attribute__((packed));

#define WHILE_COUNT_US(__X,__TIME)	\
	{	\
		uint32_t __Counter=0;	\
		while(__X)	\
		{	\
			_delay_us(1);	\
			__Counter++;	\
			if (__Counter>=__TIME)	\
				return false;	\
		}	\
	}

class NEO6M {
public:
	NEO6M();
	virtual ~NEO6M();

	bool Initialize(Usart* Serial);
	bool Status_Read();
	bool ECEF_Read();
	bool NED_VEL_Read();
	bool is_position_valid();
	int32_t	ECEF_X;
	int32_t	ECEF_Y;
	int32_t	ECEF_Z;
	int32_t	NED_Vel_N;
	int32_t	NED_Vel_E;
	int32_t	NED_Vel_D;
	uint8_t	flags;	//Navigation Status Flags



private:
	Usart* Serial;
	bool Verify_Checksun(uint8_t *msg, uint8_t len);
	bool UBX_Polling(uint8_t* UBX_msg , void* structure_data, uint8_t payload_length);
	bool UBX_Send(uint8_t *msg, uint8_t len , bool ack=false);

};

#endif /* AVIONICA_SOURCE_NEO6M_H_ */
