/*
 * core.cpp
 *
 *  Created on: 19 de Jan de 2019
 *      Author: Eduardo Lacerda Campos
 */


#include "stdlib.h"
#include <inttypes.h>


#include <avr/io.h>
#include <util/delay.h>
#include "OV7670.h"
#include "MCP23017.h"

#include <Wire.h>
#include "Usart.h"
#include "DS3231.h"

/***************************
Pinout - Arduino 1
ADC6
ADC7
AREF
PB0 ---> Enable Sensors
PB1 ---> Enable Arduino 2
PB2 ---> Enable Camera
PB3
PB4
PB5 ---> Led
PB6 <--- XTAL1
PB7 <--- XTAL2
PC0
PC1
PC2
PC3
PC4 <--> Comunicação I2C Data
PC5 ---> Comunicação I2C CLK
PC6 <--- RESET
PD0 <--- SX1276 Tx
PD1 ---> SX1276 Rx
PD2 ---> SX1276 M0 Lora
PD3 ---> SX1276 M1 Lora
PD4 <--- SX1276 AUX Lora
PD5
PD6
PD7


Pinout - Arduino 2
ADC6 <--- Temperature sensor LM35
ADC7 <--- Battery Voltage
AREF
PB0 <---
PB1 <---
PB2 ---> SPI SS
PB3 ---> SPI MOSI
PB4 <--- SPI MISO
PB5 ---> Led + SPI SCK
PB6 <--- XTAL1
PB7 <--- XTAL2
PC0 ---> AL422_WRST (Write Reset Input: initializes the write address to 0)
PC1 ---> AL422_RCK  (Read Clock Input)
PC2 ---> AL422_RRST (Write Reset Input: initializes the read address to 0)
PC3 ---> AL422_WEN  (Write Enable Input)
PC4 <--> Comunicação I2C Data
PC5 ---> Comunicação I2C CLK
PC6 <--- RESET
PD0 <--- NEO6M Tx
PD1 ---> NEO6M Rx
PD2 ---> AL422_OE ( When OE is pulled high, data output is disabled and the output pins remain at high impedance status)
PD3 <---
PD4 <---
PD5 <---
PD6 ---> OV7670_PWRD (1: Power down mode 0:Normal)
PD7 <--- OV7670_VSYNC

MCP23017
GPIOB0 <--- AL422_DO0
GPIOB1 <--- AL422_DO1
GPIOB2 <--- AL422_DO2
GPIOB3 <--- AL422_DO3
GPIOB4 <--- AL422_DO4
GPIOB5 <--- AL422_DO5
GPIOB6 <--- AL422_DO6
GPIOB7 <--- AL422_DO7

*******************************************
PinOut para testes temporários

Pinout - Arduino 2
ADC6 <--- Temperature sensor LM35
ADC7 <--- Battery Voltage
AREF
PB0 <--- AL422_DO0
PB1 <--- AL422_DO1
PB2 <--- AL422_DO2
PB3 <--- AL422_DO3
PB4 <---
PB5 ---> Led + SPI SCK
PB6 <--- XTAL1
PB7 <--- XTAL2
PC0 <--- AL422_DO4
PC1 <--- AL422_DO5
PC2 <--- AL422_DO6
PC3 <--- AL422_DO7
PC4 <--> Comunicação I2C Data
PC5 ---> Comunicação I2C CLK
PC6 <--- RESET
PD0 <--- NEO6M Tx
PD1 ---> NEO6M Rx
PD2 ---> AL422_RRST (Write Reset Input: initializes the read address to 0)
PD3 ---> AL422_WRST (Write Reset Input: initializes the write address to 0)
PD4 ---> AL422_WEN  (Write Enable Input)
PD5 ---> AL422_RCK  (Read Clock Input)
PD6 ---> RESET OV7670
PD7 <--- OV7670_VSYNC

GND = OV7670_PWRD
GND = AL422_OE ( When OE is pulled high, data output is disabled and the output pins remain at high impedance status)
****************************/
// I2C used address
// OV7670   0x21 (0x42 right shifted 1)
// DS3231   0x68
// ADXL345  0x53
// HMC5883L 0x1E
// ITG_3205 0x68 !!!!! Problem, same address !!!!!
// PCF8574  0x20 <--> 0x27
// MS5607	0x76 or 0x77




DS3231 rtc;
OV7670 Can;
MCP23017 GPIO(0);
//SX1276 LoRa;

//TODO - Criar uma rotina de tratamento de falhas.
void Failure(uint8_t code){

	for(;;)
	{
		PORTB &= ~(1<<PB5);
		_delay_ms(1000);

		for(uint8_t idx=0 ; idx<code; idx++)
		{
			PINB = (1<<PB5);
			_delay_ms(200);
			PINB = (1<<PB5);
			_delay_ms(200);
		}
	}


}

void Save(uint8_t b){
	USART.writeByte(b);
}

uint8_t Read_GPIO(){

	uint8_t val;

	if (!GPIO.Read_Byte(&val))
	{
		Failure(5);
	}

	return val;
}


void setup() {


	_delay_ms(1000);
	// initialize the digital pin as an output.
	// Pin 13 has an LED connected on most Arduino boards:
	DDRB   = (1<<PB5);
	PORTB &= ~(1<<PB5);


	// initialize serial communication
	USART.begin(2000000);
//	USART.write("Serial is ready\r\n");

	//LoRa.Initialize(&USART);

	Wire.begin();
	if(rtc.Initialize(&Wire)==false)
	{
		Failure(1);
	}

//	DateTime rtc_date(2019,1,19,12,03,00);
//	rtc.Adjust_Time(rtc_date);

	if (!GPIO.Initialize(&Wire))
	{
		Failure(2);
	}

	if (!Can.Initialize(&Wire))
	{
		Failure(3);
	}


	Can.Save_Image = &Save;
	Can.Read_GPIO = &Read_GPIO;

	//this value gives a nice result
	Can.Contrast(0x50);

//	Can.Saturation(1);

	//TODO - That function seems to have a problem with the brightness 0
//	Can.brightness(0);

//	USART.write("Can is ready\r\n");



}


void loop() {


//	_delay_ms(1000);
//	if(rtc.Read()==false)
//	{
//		Failure();
//	}
//
//	USART.write("\r\n Time: ");
//	USART.write(rtc.d);
//	USART.write("/");
//	USART.write(rtc.m);
//	USART.write("/");
//	USART.write(rtc.y+2000);
//	USART.write("  ");
//	USART.write(rtc.hh);
//	USART.write(":");
//	USART.write(rtc.mm);
//	USART.write(":");
//	USART.write(rtc.ss);
//
//	rtc.Read_Temperature();
//	double Temp = rtc.Get_Temperature();
//	USART.write("\r\n Temperature: ");
//	USART.write(Temp,2);
//
//	rtc.Force_Temperature_Update();

	uint8_t reg;
	uint8_t val;

	if (USART.available()>0)
	{

		uint8_t out=USART.read();
		switch (out){
		case 1:
			PORTB |= (1<<PB5);
			break;
		case 2:
			PORTB &= ~(1<<PB5);
			break;
		case '1':
			Can.Capture();
			Can.Read_and_Save_Image();
			break;
		case '2':
			PORTB |= (1<<PB5);
			while(USART.available()<1);
			reg=USART.read();
			val=USART.read();
			Can.write_reg(reg, val);
			break;
		case '3':
			while(USART.available()<1);
			reg=USART.read();
			USART.writeByte(Can.read_reg(reg));

			break;
		case '5':
			Can.Hardware_Reset();
			if (!Can.Initialize(&Wire))
			{
				Failure(4);
			}
			Can.Save_Image = &Save;
			Can.Read_GPIO = &Read_GPIO;
			break;
		case '6':
			Can.Test_Petern(true);
			break;
		case '7':
			Can.Test_Petern(false);
			break;
		case '8':
			while(USART.available()<1);
			reg=USART.read();
			Can.Contrast(reg);
			break;
		case '9':
			USART.writeByte(Read_GPIO());
			break;
		case 'A':
			while(USART.available()<1);
			reg=USART.read();
			if(reg==1)
				Can.old=true;
			else
				Can.old=false;
			break;



		default:
			break;
		}

//		USART.writeByte(out);
	}


}


int main(void)
{
	setup();

	for (;;)
		loop();

	return 0;
}





