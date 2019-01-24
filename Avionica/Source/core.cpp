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

#include <Wire.h>
#include "Usart.h"
#include "DS3231.h"

/***************************
Pinout - Arduino 1
ADC6 <--- Leitura do LM35
ADC7
AREF
PB0
PB1
PB2
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
ADC6
ADC7
AREF
PB0
PB1
PB2
PB3
PB4
PB5 ---> Led
PB6 <--- XTAL1
PB7 <--- XTAL2
PC0
PC1
PC2
PC3
PC4
PC5
PC6
PD0
PD1
PD2
PD3
PD4
PD5
PD6
PD7

****************************/




DS3231 rtc;

//TODO - Criar uma rotira de tratamento de falhas.
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
	DDRB=(1<<PB5);
	PORTB &= ~(1<<PB5);


	// initialize serial communication
	USART.begin(57600);
	USART.write("Serial is ready");

	Wire.begin();
	if(rtc.Initialize(&Wire)==false)
	{
		Failure();
	}
	//DateTime rtc_date(2019,1,19,12,03,00);
	//rtc.Adjust_Time(rtc_date);

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


}



int main(void)
{
	setup();

	for (;;)
		loop();

	return 0;
}





