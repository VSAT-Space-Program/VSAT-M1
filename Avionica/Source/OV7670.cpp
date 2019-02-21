/*
 * OV7670.cpp
 *
 *  Created on: 3 de fev de 2019
 *      Author: Eduardo Lacerda Campos
 *
 */

#include "OV7670.h"


OV7670::OV7670() {
	Wire= NULL;
}

OV7670::~OV7670() {
	// TODO Auto-generated destructor stub
}


bool OV7670::Initialize(TwoWire* Wire){

	this->Wire = Wire;

	//Data pins as input
	DDRC &= 0xF0;
	DDRB &= 0xF0;

	//Pull up resistor on the input
//	PORTC |= 0x0F;
//	PORTB |= 0x0F;

	//Control Pins, Define pins as Output
	CONTROL_DDR |= AL422_RCK | AL422_WEN | AL422_RRST | AL422_WRST;

	//Control Pins, Define pins as input
	CONTROL_DDR &= ~(OV7670_VSYNC);
	//Pull UP resistor
//	CONTROL_PORT |= OV7670_VSYNC;

	//Set the initial value of the output pins
	CONTROL_PORT |= AL422_RRST | AL422_WRST;
	CONTROL_PORT &= ~(AL422_WEN);

	Init_mode();

	return true;


}


bool OV7670::Capture(){

	//TODO - include a timer to avoid dead look
	//VSYNC signal was inverted, thus the falling of the signal means the frame's beginning
	while(OV7670_VSYNC_IS_HIGH != 0);

	//Reset the FIFO
	WRITE_RESET;
	WRITE_ENABLE;

	//if the MCU is faster then OV7670
	while(OV7670_VSYNC_IS_HIGH == 0);

	//wait the capture process has finished
	while(OV7670_VSYNC_IS_HIGH !=0);

	//Disable the write operation on the FIFO
	WRITE_DISABLE;
	WRITE_RESET;

	return true;
}

bool OV7670::Read_image(){


	READ_RESET;

	uint8_t Byte2,Byte1;

	//TODO  - define Size_Image as the image setup
	uint32_t Size_Image =(uint32_t)320*(uint32_t)240;

	//SAVE BMP_HEADER
	for(uint32_t idx=0; idx< sizeof(BMP_HEADER_QVGA);idx++)
		(*Save_Image)(pgm_read_byte(&(BMP_HEADER_QVGA[idx])));

	for(uint32_t idx=0; idx< Size_Image;idx++){

		Byte1 = Read_one_byte();
		Byte2 = Read_one_byte();

		(*Save_Image)(Byte1);
		(*Save_Image)(Byte2);

		//TODO - Salvar no cartão SD
	}

	return true;
}

uint8_t OV7670::Read_one_byte() {
	uint8_t b;

	READ_CLOCK_HIGH;
	b = ((0x0F & PINB) | (PINC<<4));
	READ_CLOCK_LOW;

	return b;
}

//Reset All Register Values to the default
bool OV7670::reset() {

	uint8_t ret=Send_SCCB(OV7670_I2C_ADDR, REG_COM7, COM7_RESET);

	if(ret!=0)
		return false;

	_delay_ms(500);

	//read any register to verify if the CI is working
	ret = Read_SCCB(OV7670_I2C_ADDR, 0x01);
	if (ret!= 0x80)
		return false;

	return true;
}


bool OV7670::Init_mode() {

	//reset the camera
	if (!reset())
		return false;

//	Send_SCCB(OV7670_I2C_ADDR, REG_COM7, COM7_RGB | COM7_QQVGA);
//	transfer_regvals(ov7670_fmt_rgb565);
//	transfer_regvals(ov7670_qqvga);
//	transfer_regvals(ov7670_default_old);


	transfer_regvals(ov7670_default);
	transfer_regvals(ov7670_fmt_QVGA);
	transfer_regvals(ov7670_fmt_rgb565);

	Send_SCCB(OV7670_I2C_ADDR, REG_MVFP, MVFP_MIRROR);

	Send_SCCB(OV7670_I2C_ADDR, REG_CONTRAST, 0x90);

//	transfer_regvals(ov7670_default);
//	transfer_regvals(ov7670_QVGA_RGB565);


	//the VSYNC is connected to the FIFO write reset. This reset occur when the level is low
	init_negative_vsync();

	return true;

}

// -2 (dark) to +2 (bright)
bool OV7670::brightness(int8_t value) {
	static const uint8_t values[] = {0xb0, 0x98, 0x00, 0x18, 0x30};

	value = min(max((value + 2), 0), 4);
	uint8_t ret=Send_SCCB(OV7670_I2C_ADDR, REG_BRIGHT, values[value]);

	if(ret!=0)
		return false;

	return true;
}

bool OV7670::Light_Mode(LightModeEL value){

	switch (value){
	case LightModeEL::Auto:
		Send_SCCB(OV7670_I2C_ADDR, REG_COM8, 0xe7);
		break;
	default:
		return false;

	}
	return true;

}

struct regval_list Saturation_m2[]={
		{0x4f, 0x40},
		{0x50, 0x40},
		{0x51, 0x00},
		{0x52, 0x11},
		{0x53, 0x2f},
		{0x54, 0x40},
		{0x58, 0x9e},
		{EM , EM}
};

bool OV7670::Saturation(int8_t value){

	return transfer_regvals(Saturation_m2);

}

/**
 * transfers a regval list via SCCB to camera
 *
 * true: success
 * false: failure
 */
bool OV7670::transfer_regvals(struct regval_list *list) {
	uint8_t ret = 0;
	uint8_t i = 0;

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


void OV7670::write_reg(uint8_t reg, uint8_t val){

	Send_SCCB(OV7670_I2C_ADDR,reg,val);

}


uint8_t OV7670::read_reg(uint8_t reg){

	return Read_SCCB(OV7670_I2C_ADDR,reg);

}

/*************************************************************************
* Output   0 .. success
*          1 .. length to long for buffer
*          2 .. address send, NACK received
*          3 .. data send, NACK received
*          4 .. other twi error (lost bus arbitration, bus error, ..)
**************************************************************************/
uint8_t OV7670::Send_SCCB(uint8_t slave_address, uint8_t address, uint8_t data) {

	//To make SCCB compatible with the I2C protocol, the address should be right shifted to compensate the left shift in the I2C protocol
	Wire->beginTransmission(slave_address>>1);
	Wire->write(address);
	Wire->write(data);
	return Wire->endTransmission(true);

}

/*************************************************************************
* Output   0 .. success
*          1 .. length to long for buffer
*          2 .. address send, NACK received
*          3 .. data send, NACK received
*          4 .. other twi error (lost bus arbitration, bus error, ..)
**************************************************************************/
uint8_t OV7670::Read_SCCB(uint8_t slave_address, uint8_t address) {

	if (Wire==NULL)
		return 0;

	//To make SCCB compatible with the I2C protocol, the address should be right shifted to compensate the left shift in the I2C protocol
	Wire->beginTransmission(slave_address>>1);
	Wire->write(address);
	Wire->endTransmission(true);

	Wire->requestFrom(slave_address>>1,1);

	int16_t ret = Wire->read();

	if (ret == -1)
		return 0;

	return (uint8_t)ret;



}

bool OV7670::Test_Petern(){

	Send_SCCB(OV7670_I2C_ADDR, REG_COM7, COM7_RGB | COM7_QVGA | 0x02);
	Send_SCCB(OV7670_I2C_ADDR, REG_SCALING_YSC, SCALING_YSC_QVGA | 0x80);

	return true;

}



bool OV7670::init_negative_vsync() {

	uint8_t ret=Send_SCCB(OV7670_I2C_ADDR, REG_COM10, COM10_VS_NEG);

	if (ret!=0)
		return false;

	return true;
}

uint8_t OV7670::init_default_values() {
	return transfer_regvals(ov7670_default);
}
