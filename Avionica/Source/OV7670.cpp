/*
 * OV7670.cpp
 *
 *  Created on: 3 de fev de 2019
 *      Author: educampos
 */

#include "OV7670.h"

OV7670::OV7670() {
	// TODO Auto-generated constructor stub

}

OV7670::~OV7670() {
	// TODO Auto-generated destructor stub
}


bool OV7670::Initialize(TwoWire* Wire){

	this->Wire = Wire;

	//Data pins
	//Define the pins 0 and 1 as input
	DDRB &= ~(0b00000011);

	//Define the pins 7,6,5,4,3 e 2 as input
	DDRD &= 0b00000011;

	//Control Pins, Define as Output
	CONTROL_DDR |= AL422_RCK | AL422_WEN | AL422_RRST | AL422_WRST;

	//Set the initial value
	CONTROL_PORT = AL422_RRST | AL422_WRST;
	CONTROL_PORT &= ~(AL422_WEN);

	return reset(MODE_YUV);


}

/**
 * returns true if camera was initialized succesful
 * mode: MODE_RGB444, MODE_RGB555, MODE_RGB565, MODE_YUV
 */
bool OV7670::reset(uint8_t mode) {

	camera_mode = mode;

	if (!init_camera_reset())
		return false;// something fails

//	switch (camera_mode) {
//	case MODE_RGB444:
//		ret = init_rgb444_qqvga();
//		if (ret != 1) return ret;
//		break;
//	case MODE_RGB555:
//		ret = init_rgb555_qqvga();
//		if (ret != 1) return ret;
//		break;
//	case MODE_RGB565:
//		ret = init_rgb565_qqvga();
//		if (ret != 1) return ret;
//		break;
//	case MODE_YUV:
//		ret = init_yuv_qqvga();
//		if (ret != 1) return ret;
//		break;
//
//	}
//
//	init_negative_vsync();
//	ret = init_default_values();
//
	return true;
}

//Reset All Register Values
bool OV7670::init_camera_reset() {

	uint8_t ret=Send_SCCB(OV7670_I2C_ADDR, REG_COM7, COM7_RESET);

	if(ret!=0)
		return false;

	_delay_ms(200);

	return true;
}

/**
 * transfers a regval list via SCCB to camera
 *
 * true: success
 * false: failure
 */
uint8_t OV7670::transfer_regvals(struct regval_list *list) {
	uint8_t ret = 0;
	uint8_t i = 0;

	//TODO dangerous dead-lock. Should be avoided
	while(true) {
		// end marker check
		if ((list[i].reg_num == EM) && (list[i].value == EM)) {
			break;
		}

		//Send SCCB
		ret=Send_SCCB(OV7670_I2C_ADDR, list[i].reg_num, list[i].value);
		if(ret!=0)
			return false;


		// delay for reset command
		if ((list[i].reg_num == REG_COM7) && (list[i].value == COM7_RESET)) {
			_delay_ms(200);
		}

		i++;
	}

	return true;
}

/*************************************************************************
* Output   0 .. success
*          1 .. length to long for buffer
*          2 .. address send, NACK received
*          3 .. data send, NACK received
*          4 .. other twi error (lost bus arbitration, bus error, ..)
**************************************************************************/
uint8_t OV7670::Send_SCCB(uint8_t slave_address, uint8_t address, uint8_t data) {

	//To make SCCB compatible with the I2C protocol, the address should be right shifted to compensate the left shift
	Wire->beginTransmission(slave_address>>1);
	Wire->write(address);
	Wire->write(data);
	return Wire->endTransmission(true);

}

