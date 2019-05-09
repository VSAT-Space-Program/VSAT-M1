/*
 * SX1276.h
 *
 *  Created on: 20 de jan de 2019
 *      Authors: Eduardo Lacerda Campos e Saulo Aislan da Silva Eleutério
 *
 *  Driver para acessar a comunicação LoRa por meio do CI SX1276
 *
 */

#ifndef AVIONICA_SOURCE_SX1276_H_
#define AVIONICA_SOURCE_SX1276_H_

#include "Usart.h"
#include <util/delay.h>

//Pin Out

// Arduino Tx PD0 ---> SX1276 Rx
// Arduino Rx PD1 <--- SX1276 Tx
// Arduino PD2 ---> SX1276 M0
// Arduino PD3 ---> SX1276 M1
// Arduino PD4 <--- SX1276 AUX


#define M0 PD2
#define M1 PD3
#define AUX PD4
#define AUX_IS_HIGH (PIND & (1<<AUX))

//Operating Frequency
//0x00 to 0x1F corresponding to 900 to 931 MHz

#define MAX_TX_SIZE 58

//Air data rate
#define AIR_BPS_300 	0x00
#define AIR_BPS_1200 	0x01
#define AIR_BPS_2400 	0x02
#define AIR_BPS_4800 	0x03
#define AIR_BPS_9600 	0x04
#define AIR_BPS_19300 	0x05

//USART data rate
#define USART_BPS_1200 	 0x00
#define USART_BPS_2400 	 0x08
#define USART_BPS_4800 	 0x10
#define USART_BPS_9600 	 0x18
#define USART_BPS_19300  0x20
#define USART_BPS_38400  0x28
#define USART_BPS_57600  0x30
#define USART_BPS_115200 0x38

#define FIXED_MODE (1<<7)
#define PULL_UP_MODE (1<<6)
#define WAKE_UP_TIME_1250MS (00100000)
#define TURN_FEC (1<<2)
#define POWER_30DB 0u


//UART parity bit
#define P_8N1 0x00

//MACROS
#define WHILE_COUNT(__X)	\
	{	\
		uint16_t __Counter=0;	\
		while(__X)	\
		{	\
			_delay_ms(1);	\
			__Counter++;	\
			if (__Counter>=1000)	\
				return false;	\
		}	\
	}

class SX1276 {
public:
	SX1276();
	virtual ~SX1276();
	bool Send(uint16_t Adrress, uint8_t Channel, uint8_t* data, uint8_t Lenght);
	bool Received(uint8_t* data, uint8_t* Lenght);
	bool Default_Setup(void);

	//Channel = 0x0F  -> Default 915 MHz
	bool Initialize(Usart* Serial,uint16_t Address, uint8_t Channel=0x0F);

private:
	Usart* Serial;
	uint16_t Address;
	uint8_t Channel;
	bool ReadBytes(uint8_t* data, uint8_t Lenght);
};

/*******************************************************
* Structures
*******************************************************/

#pragma pack(push, 1)
struct CFG_struct {
  uint8_t HEAD;
  uint8_t ADDH;
  uint8_t ADDL;
  uint8_t SPED;
  uint8_t CHANNEL;
  uint8_t OPTION;
};
#pragma pack(pop)

#endif /* AVIONICA_SOURCE_SX1276_H_ */
