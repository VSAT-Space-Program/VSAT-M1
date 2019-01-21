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
// Arduino ?? PD2 ---> SX1276 M0
// Arduino ?? PD3 ---> SX1276 M1
// Arduino ?? PD4 <--- SX1276 AUX


#define M0 PD2
#define M1 PD3
#define AUX PD4

#define ADDH 0x00
#define ADDL 0x01

//Operating Frequency
//0x00 to 0xFF corresponding to 900 to 931 MHz
//Default 915 MHz
#define CHAN

class SX1276 {
public:
	SX1276();
	virtual ~SX1276();

	bool Initialize(Usart* Serial);

private:
	Usart* Serial;
};

#endif /* AVIONICA_SOURCE_SX1276_H_ */
