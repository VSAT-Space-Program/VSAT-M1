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
//#define MODE_YUV 	3

#define CONTROL_DDR			DDRC
#define CONTROL_PORT		PORTC
#define CONTROL_PIN			PINC


#define AL422_WRST          _BV(PINC0)          // Write Reset (active low)
#define AL422_RCK           _BV(PINC1)          // Read clock
#define AL422_RRST          _BV(PINC2)          // Read Reset (active low)
#define AL422_WEN           _BV(PINC3)          // Write Enable (active low) - labeled WR on camera pin
#define OV7670_VSYNC        _BV(PIND4)
#define OV7670_VSYNC_IS_HIGH (PORTD & _BV(PIND4))

#define WRITE_ENABLE CONTROL_PORT |= AL422_WEN
#define WRITE_DISABLE CONTROL_PORT &= ~(AL422_WEN)

#define READ_CLOCK_HIGH CONTROL_PORT |= AL422_RCK
#define READ_CLOCK_LOW CONTROL_PORT &= ~(AL422_RCK)

#define READ_RESET_HIGH CONTROL_PORT |= AL422_RRST
#define READ_RESET_LOW CONTROL_PORT &= ~(AL422_RRST)

#define WRITE_RESET_HIGH CONTROL_PORT |= AL422_WRST
#define WRITE_RESET_LOW CONTROL_PORT &= ~(AL422_WRST)

#define READ_RESET READ_RESET_LOW; READ_CLOCK_HIGH; READ_RESET_HIGH; READ_CLOCK_LOW;

#define WRITE_RESET WRITE_RESET_LOW; WRITE_RESET_HIGH;


//Macros
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

class OV7670 {
public:
	OV7670();
	virtual ~OV7670();
	bool Initialize(TwoWire* Wire);
	bool brightness(int8_t value);
	bool Capture();
	bool Read_image();


private:
	TwoWire* Wire;
	uint8_t camera_mode;
	bool init_camera_reset();
	uint8_t transfer_regvals(struct regval_list *list);
	uint8_t Send_SCCB(uint8_t slave_address, uint8_t address, uint8_t data);
	bool reset(uint8_t mode);
	uint8_t init_rgb444_qqvga(void);
	uint8_t init_rgb555_qqvga(void);
	uint8_t init_rgb565_qqvga(void);
	uint8_t init_yuv_qqvga(void);
	bool init_negative_vsync();
	uint8_t init_default_values();
	uint8_t Read_one_byte();

};

#endif /* OV7670_H_ */
