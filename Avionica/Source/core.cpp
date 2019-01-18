/*
 * core.cpp
 *
 *  Created on: 26 de mar de 2017
 *      Author: educampos
 */


#include "stdlib.h"
#include <inttypes.h>


#include <avr/io.h>
#include <util/delay.h>

#include <Wire.h>
#include "DS3221.h"
#include "Usart.h"

//LED PB5. Pino 13 no arduino uno
//SDA PC4. Pino AD4 no arduino uno
//SCL PC5. pino AD5 no arduino uno


DS3221 rtc;

void setup() {

	// initialize serial communication
	USART.begin(57600);
	USART.write("Serial is ready");

	Wire.begin();
	rtc.Initialize(&Wire);

}


void loop() {


	_delay_ms(1000);
	rtc.Read();

	USART.write("\r\n");
	USART.write(rtc.ss);


}



int main(void)
{
	setup();

	for (;;)
		loop();

	return 0;
}





