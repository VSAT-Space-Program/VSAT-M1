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
PC6
PD0 ---> SX1276 Rx
PD1 <--- SX1276 Tx
PD2 ---> SX1276 M0 Lora
PD3 ---> SX1276 M1 Lora
PD4 <--- SX1276 AUX Lora
PD5
PD6
PD7


Pinout - Arduino 2
ADC6 <--- Leitura do LM35
ADC7
AREF
PB0 <--- AL422_DO0
PB1 <--- AL422_DO1
PB2 ---> SPI SS
PB3 ---> SPI MOSI
PB4 <--- SPI MISO
PB5 ---> Led + SPI SCK
PB6 <--- XTAL1
PB7 <--- XTAL2
PC0 ---> AL422_WRST
PC1 ---> AL422_RCK
PC2 ---> AL422_RRST
PC3 ---> AL422_WEN
PC4 <--> Comunicação I2C Data
PC5 ---> Comunicação I2C CLK
PC6
PD0 ---> NEO6M Rx
PD1 <--- NEO6M Tx
PD2 <--- AL422_DO2
PD3 <--- AL422_DO3
PD4 <--- AL422_DO4
PD5 <--- AL422_DO5
PD6 <--- AL422_DO6
PD7 <--- AL422_DO7


****************************/




DS3231 rtc;
OV7670 Can;
//SX1276 LoRa;

//TODO - Criar uma rotina de tratamento de falhas.
void Failure(){

	for(;;)
	{
		PORTB &= ~(1<<PB5);
		_delay_ms(1000);
		PINB = (1<<PB5);
		_delay_ms(200);
		PINB = (1<<PB5);
		_delay_ms(200);
		PINB = (1<<PB5);
		_delay_ms(200);
	}


}


void setup() {


	// initialize the digital pin as an output.
	// Pin 13 has an LED connected on most Arduino boards:
	DDRB   = (1<<PB5);
	PORTB &= ~(1<<PB5);



	// initialize serial communication
	USART.begin(9600);
	USART.write("Serial is ready\r\n");

	//LoRa.Initialize(&USART);

	Wire.begin();
	if(rtc.Initialize(&Wire)==false)
	{
		Failure();
	}
//	DateTime rtc_date(2019,1,19,12,03,00);
//	rtc.Adjust_Time(rtc_date);


	if (!Can.Initialize(&Wire))
	{
		Failure();
	}

	USART.write("Can is ready\r\n");


}


void loop() {


	_delay_ms(1000);
	if(rtc.Read()==false)
	{
		Failure();
	}

	USART.write("\r\n Time: ");
	USART.write(rtc.d);
	USART.write("/");
	USART.write(rtc.m);
	USART.write("/");
	USART.write(rtc.y+2000);
	USART.write("  ");
	USART.write(rtc.hh);
	USART.write(":");
	USART.write(rtc.mm);
	USART.write(":");
	USART.write(rtc.ss);

	rtc.Read_Temperature();
	double Temp = rtc.Get_Temperature();
	USART.write("\r\n Temperature: ");
	USART.write(Temp,2);

	rtc.Force_Temperature_Update();
//	USART.write(rtc.Temp);


//	if Lora.Read(data,len)
//		{
//		if (data == Hand_shake)
//			Led 1 on Led off
//			Buzzer
//		if (data == Posição)
//			Led 2 on ... off
//
//		}



}

void loop2(){

}

int main(void)
{
	setup();

	for (;;)
		loop();
//		loop2();

	return 0;
}





