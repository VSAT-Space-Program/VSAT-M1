/*
 * OV7670.h
 *
 *  Created on: 3 de fev de 2019
 *      Author: Eduardo Lacerda Campos
 *
 * The OV7670/OV7171 CAMERACHIPS has an image array size of 656 columns by 488 rows (320,128 pixels)
 *
 * The pixel cells themselves are identical, but have RGB color filters arranged in a row-alternating
 * BG/GR Bayer Pattern.
 *
 * The Shield comes with a 12MHz crystal oscillator
 *
 *  * The example source code is:
 * https://github.com/torvalds/linux/blob/master/drivers/media/i2c/ov7670.c
 */

#ifndef OV7670_H_
#define OV7670_H_

/*
 * board PINTOU
 *
 *  Vcc 		|  	| GND
 *  SCK			|	| SDATA
 *  VSYNC		|	| HREF
 *  D7			|	| D6
 *  D5			|	| D4
 *  D3			|	| D2
 *  D1			|	| D0
 *  RESET		|	| PWDN (1: Power down mode 0:Normal)
 *  STROBE		|	| FIFO RCK
 *	FIFO WR		|	| FIFO OE
 *	FIFO WRST	|	| FIFO RRST
 */
#include "stdlib.h"
#include <inttypes.h>
#include <Wire.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/io.h>


#include "camera_config.h"

#define MODE_RGB555 1
#define MODE_RGB565 2
//#define MODE_YUV 	3

#define CONTROL_DDR	DDRD
#define CONTROL_PIN	PIND
#define CONTROL_PORT PORTD

#define AL422_RRST          _BV(PD2)          // Read Reset (active low)
#define AL422_WRST          _BV(PD3)          // Write Reset (active low)
#define AL422_WEN           _BV(PD4)          // Write Enable (active low) - labeled WR on camera pin
#define AL422_RCK           _BV(PD5)          // Read clock

#define OV7670_RESET        _BV(PD6)
#define OV7670_VSYNC        _BV(PD7)
#define OV7670_VSYNC_IS_HIGH (PIND & OV7670_VSYNC)

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

//BMP Hearder for image with size=640x480 RGB=565
//const uint8_t BMP_HEADER_VGA[] PROGMEM=
//	{0x42,0x4d,0x46,0x60,0x09,0x00,0x00,0x00,0x00,0x00,0x46,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x80,0x02,0x00,0x00,0xe0,0x01,0x00,0x00,0x01,
//	 0x00,0x10,0x00,0x03,0x00,0x00,0x00,0x00,0x60,0x09,0x00,0x13,0x0b,0x00,0x00,0x13,0x0b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//	 0x00,0xf8,0x00,0x00,0xe0,0x07,0x00,0x00,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

const uint8_t BMP_HEADER_QVGA[] PROGMEM=
	{0x42,0x4D,0x46,0x58,0x02,0x00,0x00,0x00,0x00,0x00,0x46,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x40,0x01,0x00,0x00,0xF0,0x00,0x00,0x00,0x01,
	 0x00,0x10,0x00,0x03,0x00,0x00,0x00,0x00,0x58,0x02,0x00,0x13,0x0B,0x00,0x00,0x13,0x0B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	 0x00,0xF8,0x00,0x00,0xE0,0x07,0x00,0x00,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

enum  LightModeEL {
  Auto,
  Synny,
  Cloudy,
  Office,
  Home
};

class OV7670 {
public:
	OV7670();
	virtual ~OV7670();
	bool Initialize(TwoWire* Wire);
	bool brightness(int8_t value);
	bool Capture();
	bool Read_and_Save_Image();
	bool Test_Petern(bool flag);
	bool Light_Mode(LightModeEL value);
	bool Night_Mode(bool flag);
	bool Saturation(float value);
	bool Hardware_Reset();
	bool Contrast(uint8_t value);

	void (*Save_Image)(uint8_t b);
	uint8_t (*Read_GPIO)();

	void write_reg(uint8_t reg, uint8_t val);
	uint8_t read_reg(uint8_t reg);

	bool old;

private:
	TwoWire* Wire;
	bool transfer_regvals(struct regval_list *list);
	uint8_t Send_SCCB(uint8_t slave_address, uint8_t address, uint8_t data);
	uint8_t Read_SCCB(uint8_t slave_address, uint8_t address);
	bool reset();
	bool Init_mode();
	bool init_negative_vsync();
	//uint8_t init_default_values();
	uint8_t Read_one_byte();

};



#endif /* OV7670_H_ */
