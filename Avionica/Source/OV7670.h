/*
 * OV7670.h
 *
 *  Created on: 3 de fev de 2019
 *      Author: Eduardo Lacerda Campos
 */

#ifndef OV7670_H_
#define OV7670_H_

#include <Wire.h>
#include <util/delay.h>

#include "camera_config.h"

#define MODE_RGB444 0
#define MODE_RGB555 1
#define MODE_RGB565 2
#define MODE_YUV 	3

#define CONTROL_DDR			DDRC
#define CONTROL_PORT		PORTC
#define CONTROL_PIN			PINC


#define AL422_WRST          _BV(PINC0)          // Write Reset (active low)
#define AL422_RCK           _BV(PINC1)          // Read clock
#define AL422_RRST          _BV(PINC2)          // Read Reset (active low)
#define AL422_WEN           _BV(PINC3)          // Write Enable (active low) - labeled WR on camera pin


// TODO - attach OE and PWDN to the arduino 1

class OV7670 {
public:
	OV7670();
	virtual ~OV7670();
	bool Initialize(TwoWire* Wire);


private:
	TwoWire* Wire;
	uint8_t camera_mode;
	bool init_camera_reset();
	uint8_t transfer_regvals(struct regval_list *list);
	uint8_t Send_I2C(uint8_t slave_address, uint8_t address, uint8_t data);
	bool reset(uint8_t mode);
};

#endif /* OV7670_H_ */
