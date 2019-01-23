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
Descrição dos pinos utilizados
Arduino PB5 ---> Led no arduino nano
Arduino SDA PC4 <--> Comunicação I2C
Arduino SCL PC5 ---> Comunicação I2C
Arduino PC0 AD0 <--- Para a leitura do LM35
Arduino PC1 AD1 <--- Para o inversor de tensão
Arduino Tx PD0 ---> SX1276 Rx
Arduino Rx PD1 <--- SX1276 Tx
Arduino PD2 ---> SX1276 M0 Lora
Arduino PD3 ---> SX1276 M1 Lora
Arduino PD4 <--- SX1276 AUX Lora
****************************/




DS3231 rtc;


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





