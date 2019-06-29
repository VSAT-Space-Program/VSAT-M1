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
	Save_Image = NULL;
	old=true;
}

OV7670::~OV7670() {
	// TODO Auto-generated destructor stub
}

/***********************************************************************
Initialize the OV7670 registers to operate with RGB555 and QVGA
Input:  TWI object pointer
Output: false -> Failure
		true  -> Success
************************************************************************/

bool OV7670::Initialize(TwoWire* Wire){

	this->Wire = Wire;

	//Data pins as input
	//Using the MCP23017

	//Control Pins, Define pins as Output
	CONTROL_DDR |= AL422_RCK | AL422_WEN | AL422_RRST | AL422_WRST | OV7670_RESET;

	//Control Pins, Define pins as input
	CONTROL_DDR &= ~(OV7670_VSYNC);

	//Pull UP resistor
//	CONTROL_PORT |= OV7670_VSYNC;

	//Set the initial value of the output pins
	CONTROL_PORT |= AL422_RRST | AL422_WRST | OV7670_RESET;
	CONTROL_PORT &= ~(AL422_WEN);

	//wait for the camera to finish its startup
	_delay_ms(500);

	if(!Init_mode())
		return false;

	return true;


}

/***********************************************************************
Capture and store on the buffer
Input: None
Output: false -> Failure
		true  -> Success
************************************************************************/
bool OV7670::Capture(){

	//TODO - include a timer to avoid dead look
	//VSYNC signal was inverted, thus the falling of the signal means the frame's beginning
	WHILE_COUNT_US(OV7670_VSYNC_IS_HIGH != 0,100000);

	//Reset the FIFO
	WRITE_RESET;
	WRITE_ENABLE;

	//if the MCU is faster then OV7670
	WHILE_COUNT_US(OV7670_VSYNC_IS_HIGH == 0,10000);

	//wait the capture process has finished
	WHILE_COUNT_US(OV7670_VSYNC_IS_HIGH !=0,100000);

	//Disable the write operation on the FIFO
	WRITE_DISABLE;
	//Reset the FIFO
	WRITE_RESET;

	return true;
}


/***********************************************************************
Read all image's bytes and send it to the next handle function
Input: None
Output: false -> Failure
		true  -> Success
************************************************************************/
bool OV7670::Read_and_Save_Image(){


	READ_RESET;

	uint8_t Byte2,Byte1;

	//TODO  - define Size_Image in the image setup
	uint32_t Size_Image =320UL * 240UL;

	//Save BMP_HEADER
	for(uint32_t idx=0; idx< sizeof(BMP_HEADER_QVGA);idx++)
		(*Save_Image)(pgm_read_byte(&(BMP_HEADER_QVGA[idx])));


	//http://www.arducam.com/rgb565-format-issues/
	for(uint32_t idx=0; idx< Size_Image;idx++){

		//read one pixel
		Byte1 = Read_one_byte();
		Byte2 = Read_one_byte();

		//save the pixel
		(*Save_Image)(Byte2);
		(*Save_Image)(Byte1);


	}

	return true;
}

uint8_t OV7670::Read_one_byte() {
	uint8_t b;

	READ_CLOCK_HIGH;
	if(old==true)
	{
		b = ((0x0F & PINB) | (0xF0 & (PINC<<4)));
	}
	else
	{
		b = (*Read_GPIO)();
	}
	READ_CLOCK_LOW;

	return b;
}
/***********************************************************************
Reset all register to the default values
Input: None
Output: false -> Failure
		true  -> Success
************************************************************************/
bool OV7670::reset() {

	uint8_t ret=Send_SCCB(OV7670_I2C_ADDR, REG_COM7, COM7_RESET);

	if(ret!=0)
		return false; //fail to reset

	_delay_ms(500);

	//read any register to verify if the CI is working
	ret = Read_SCCB(OV7670_I2C_ADDR, 0x01);

	if (ret!= 0x80)
		return false; //fail to reset

	return true; //success
}

/***********************************************************************
Initialize the OV7670 registers to operate with RGB555 and QVGA
Input: None
Output: false -> Failure
		true  -> Success
************************************************************************/
bool OV7670::Init_mode() {


	//reset the camera
	if (!reset())
		return false;

	uint16_t MID =  (Read_SCCB(OV7670_I2C_ADDR,REG_MIDH))<<8;
	MID |=  (Read_SCCB(OV7670_I2C_ADDR,REG_MIDL));


	if (MID != 0x7FA2)
		//Can not find the Ov7670
		return false;


	struct regval_list *msg;

	msg = new struct regval_list[sizeof(ov7670_default)];
	memcpy_P(msg, &ov7670_default[0], sizeof(ov7670_default));
	transfer_regvals(msg);;
	delete msg;

	msg = new struct regval_list[sizeof(ov7670_fmt_QVGA)];
	memcpy_P(msg, &ov7670_fmt_QVGA[0], sizeof(ov7670_fmt_QVGA));
	transfer_regvals(msg);
	delete msg;

	msg = new struct regval_list[sizeof(ov7670_fmt_rgb565)];
	memcpy_P(msg, &ov7670_fmt_rgb565[0], sizeof(ov7670_fmt_rgb565));
	transfer_regvals(msg);
	delete msg;


	Send_SCCB(OV7670_I2C_ADDR, REG_MVFP, MVFP_MIRROR);

	//the VSYNC is connected to the FIFO write reset. This reset occur when the level is low
	init_negative_vsync();

	return true;

}
/***********************************************************************
 Change the brightness value
Input: Values from -2 (dark) to +2 (bright)
Output: false -> Failure
		true  -> Success
************************************************************************/
bool OV7670::brightness(int8_t value) {
	static const uint8_t values[] = {0xb0, 0x98, 0x00, 0x18, 0x30};

	value = min(max((value + 2), 0), 4);
	uint8_t ret=Send_SCCB(OV7670_I2C_ADDR, REG_BRIGHT, values[value]);

	if(ret!=0)
		return false;

	return true;
}
/***********************************************************************
 Change the Light Mode
************************************************************************/
bool OV7670::Light_Mode(LightModeEL value){

	//TODO - Include other configurations
	switch (value){
	case LightModeEL::Auto:
		Send_SCCB(OV7670_I2C_ADDR, REG_COM8, 0xe7);
		break;
	default:
		return false;

	}
	return true;

}

/*************************************************************************
 * Enable o Disable the night mode
 * Input:   Flag -> True -> enable
 * 					False -> Disable
 * Output: 	true  -> success
 * 			false -> failure
 *************************************************************************/
bool OV7670::Night_Mode(bool flag){


	uint16_t Val =  Read_SCCB(OV7670_I2C_ADDR,REG_COM11);

	if(flag==true)
		Val |= COM11_NIGHT | COM11_NIGHT_FR8;
	else
		Val &= ~(COM11_NIGHT | COM11_NIGHT_FR8);


	Send_SCCB(OV7670_I2C_ADDR,REG_COM11,Val);

	return true;

}


/*************************************************************************
 * transfers the register values via SCCB to camera
 * Input:   Value -> percent multiplier from 0 to 1.11
 * Output: 	true  -> success
 * 			false -> failure
 *************************************************************************/
bool OV7670::Saturation(float value){

	//default values
	int16_t cmatrix[]={ 179, -179, 0, -61, -176, 228 };

	uint8_t signal =0;

	for(uint8_t idx=0 ; idx< CMATRIX_LEN ; idx++)
	{
		cmatrix[idx] = cmatrix[idx] * value;

		//TODO - this procedure is not necessary now, but when hue color correction is implemented
		// this procedure will be necessary
		if (cmatrix[idx]<0)
			signal |= (1<<idx);
	}

	for(uint8_t idx=0 ; idx< CMATRIX_LEN ; idx++)
	{
		Send_SCCB(OV7670_I2C_ADDR,REG_CMATRIX_1 + idx ,abs(cmatrix[idx]));
	}

	//Read and eliminate the old signal values
	uint8_t REG_MTXS_Value =  Read_SCCB(OV7670_I2C_ADDR, REG_MTXS) & 0xC0;

	//Send the new signal of the cmatrix items
	Send_SCCB(OV7670_I2C_ADDR,REG_MTXS, REG_MTXS_Value  | signal);

	return true;
}

/*************************************************************************
 * transfers the register values via SCCB to camera
 * Input:   Value 0 to 255
 * Output: 	true -> success
 * 			false -> failure
 *************************************************************************/
bool OV7670::Contrast(uint8_t value)
{
	if (Send_SCCB(OV7670_I2C_ADDR, REG_CONTRAST, value)!=0)
		return false;

	return true;
}

/*************************************************************************
 * transfers the register values via SCCB to camera
 *
 * Output:
 * 			true -> success
 * 			false -> failure
 *************************************************************************/
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

	//To make SCCB compatible with the I2C protocol, the slave_address must be right shifted
	Wire->beginTransmission(slave_address>>1);
	Wire->write(address);
	Wire->write(data);
	return Wire->endTransmission(true);

}

//TODO - review this function to return the error information
/*************************************************************************
* Output   0 .. success
*          1 .. length to long for buffer
*          2 .. address send, NACK received
*          3 .. data send, NACK received
*          4 .. other twi errors (lost bus arbitration, bus error, ..)
**************************************************************************/
uint8_t OV7670::Read_SCCB(uint8_t slave_address, uint8_t address) {

	if (Wire==NULL)
		return 0;

	//To make SCCB compatible with the I2C protocol, the slave_address must be right shifted
	Wire->beginTransmission(slave_address>>1);
	Wire->write(address);
	Wire->endTransmission(true);

	Wire->requestFrom(slave_address>>1,1);

	int16_t ret = Wire->read();

	if (ret == -1)
		return 0;

	return (uint8_t)ret;

}

/***********************************************************************
Switch on/off the pattern test
Input: 	flag: true -> Enable
			  false -> Disable

Output: false -> Failure
		true  -> Success
************************************************************************/
bool OV7670::Test_Petern(bool flag){


	uint8_t COM7_Value =  Read_SCCB(OV7670_I2C_ADDR, REG_COM7);
	uint8_t SCALING_YSC_Value =  Read_SCCB(OV7670_I2C_ADDR, REG_SCALING_YSC);

	if(flag==true){
		Send_SCCB(OV7670_I2C_ADDR, REG_COM7, COM7_Value | 0x02);
		Send_SCCB(OV7670_I2C_ADDR, REG_SCALING_YSC, SCALING_YSC_Value | 0x80);
	}
	else
	{
		Send_SCCB(OV7670_I2C_ADDR, REG_COM7, COM7_Value & ~(0x02));
		Send_SCCB(OV7670_I2C_ADDR, REG_SCALING_YSC, SCALING_YSC_Value & ~(0x80));
	}



	return true;

}


bool OV7670::init_negative_vsync() {

	uint8_t ret=Send_SCCB(OV7670_I2C_ADDR, REG_COM10, COM10_VS_NEG);

	if (ret!=0)
		return false;

	return true;
}

//uint8_t OV7670::init_default_values() {
//	return transfer_regvals(ov7670_default);
//}

/***********************************************************************
Reset the hardware by physical wire
Input: None
Output: false -> Failure
		true  -> Success
************************************************************************/
bool OV7670::Hardware_Reset(){

	CONTROL_PORT &= ~OV7670_RESET;
	_delay_ms(1000);
	CONTROL_PORT |= OV7670_RESET;

	return true;

}

