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
class NEO6M {
public:
	NEO6M();
	virtual ~NEO6M();

	void Initialize(Usart* Serial);
	void UBX_Send(uint8_t *msg, uint8_t len);
	void Poll();
private:
	Usart* Serial;
};

#endif /* AVIONICA_SOURCE_NEO6M_H_ */
