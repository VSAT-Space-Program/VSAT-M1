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

//LED PB5. Pino 13 no arduino nano
//SDA PC4. Pino AD4 no arduino nano
//SCL PC5. pino AD5 no arduino nano


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





