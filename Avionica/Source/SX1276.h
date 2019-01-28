/*
 * SX1276.h
 *
 *  Created on: 20 de jan de 2019
 *      Author: Eduardo Lacerda Campos
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

#define ADDH 0x00
#define ADDL 0x00

//Operating Frequency
//0x00 to 0x1F corresponding to 900 to 931 MHz

#define CHAN 0x0F  //Default 915 MHz

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

//UART parity bit
#define P_8N1 0x00

class SX1276 {
public:
	SX1276();
	virtual ~SX1276();
	bool Send(uint16_t Adrress, uint8_t Channel, uint8_t* data, uint8_t Lenght);
	bool Read(uint8_t* data, uint8_t* Lenght);

	bool Initialize(Usart* Serial);

private:
	Usart* Serial;
};

#endif /* AVIONICA_SOURCE_SX1276_H_ */
