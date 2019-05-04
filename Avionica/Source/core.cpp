/*
 * core.cpp
 *
 *  Created on: 19 de Jan de 2019
 *      Author: Eduardo Lacerda Campos
 */


#include "stdlib.h"
#include <stdio.h>
#include <inttypes.h>


#include <avr/io.h>
#include <util/delay.h>
#include "OV7670.h"
#include "MCP23017.h"

#include <Wire.h>
#include "Usart.h"
#include "DS3231.h"

#include "FAT/SDCard.h"
#include "FAT/FAT.h"
#include "FAT/File.h"
#include "FAT/SPISD.h"

SDCard disk(&PORTB, &DDRB, PB2);

FAT fs(&disk);
File root(&fs);
File file(&fs);

DS3231 rtc;
OV7670 Can;
MCP23017 GPIO(0);
uint8_t Fig_Count=0;
//SX1276 LoRa;

/******************************************************************
 * Blink led routine to inform the error
 ******************************************************************/

enum Error_LED {
  RCT_INIT = 1,
  GPIO_INIT = 2,
  CAN_INIT = 3,
  GPIO_READ=5,
  SDCARD_INIT=6
};

extern "C" void Failure(uint8_t code){

	SPCR=0;
	DDRB   = (1<<PB5);
	PORTB &= ~(1<<PB5);

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

/******************************************************************
 * SD Card routine to inform the error
 ******************************************************************/

void handle_error()
{
    switch(disk.get_error()){
    case SDCard::Error::CMD0:
    	printf_P(PSTR("timeout error for command CMD0\n"));
        break;
    case SDCard::Error::CMD8:
    	printf_P(PSTR("CMD8 was not accepted - not a valid SD card\n"));
        break;
    case SDCard::Error::ACMD41:
    	printf_P(PSTR("card's ACMD41 initialization process timeout\n"));
        break;
    case SDCard::Error::CMD58:
    	printf_P(PSTR("card returned an error response for CMD58 (read OCR)\n"));
        break;
    case SDCard::Error::CMD24:
    	printf_P(PSTR("card returned an error response for CMD24 (write block)\n"));
        break;
    default:
        printf_P(PSTR("Unknown error. Code %x\n"), (uint8_t)disk.get_error());
        break;
    }
}

/******************************************************************
 * Routine to save the image read from the camera
 ******************************************************************/
void Save(uint8_t b){
	USART.writeByte(b);
	file.write(&b, 1);
}

/******************************************************************
 * Read the port B from the MCP23017
 ******************************************************************/
uint8_t Read_GPIO(){

	uint8_t val;

	if (!GPIO.Read_Byte(&val))
	{
		Failure(5);
	}

	return val;
}

/******************************************************************
 * Define the instruction to the printf or printfP routines
 ******************************************************************/
int Std_putchar(char c, FILE *stream) {
    USART.writeByte(c);
    return 0;
}

/******************************************************************
 * Main setup
 ******************************************************************/
void setup() {


	_delay_ms(1000);
	// initialize the digital pin as an output.
	// Pin 13 has an LED connected on most Arduino boards:
//	DDRB   = (1<<PB5);
//	PORTB &= ~(1<<PB5);


	// initialize serial communication
	USART.begin(250000);
//	USART.begin(9600);

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

//    stdout = fdevopen(Std_putchar, NULL);
//    stdin  = fdevopen(NULL, uart_getchar);

    printf_P(PSTR("Initializing SD card...\r\n"));
    if(disk.init()){
    	printf_P(PSTR("Card connected!\r\n"));
    } else {
    	printf_P(PSTR("Card initialization failed.\r\n"));
        handle_error();
        Failure(6);
    }

    SPI_SD::set_speed();

    printf_P(PSTR("\nMounting FAT Filesystem...\n"));
    if(fs.mount()){
    	printf_P(PSTR("Filesystem mounted!\n"));
    	printf_P(PSTR("Filesystem Type FAT%i\n"), fs.get_type());
//    	printf_P(PSTR("Free Memory %i\n"), freeRam());
    } else {
    	printf_P(PSTR("Mount error.\n"));
        handle_error();
        Failure(6);
    }
    printf_P(PSTR("\nOpening filesystem root...\n"));
    if(root.open_root()){
    	printf_P(PSTR("Root is open\n"));
//    	printf_P(PSTR("Free Memory %i\n"), freeRam());
    } else {
    	printf_P(PSTR("Unable to open root\n"));
        handle_error();
        Failure(6);
    }


	if (!Can.Initialize(&Wire))
	{
		Failure(3);

	}

//	USART.write("Can ok\n");

	Can.old=false;
	Can.Save_Image = &Save;
	Can.Read_GPIO = &Read_GPIO;

	//this value gives a nice result
	Can.Contrast(0x50);

//	Can.Saturation(1);

	//TODO - That function seems to have a problem with the brightness 0
//	Can.brightness(0);

//	USART.write("Can is ready\r\n");

//	printf_P(PSTR("Setup Finished\r\n"));

    if(file.open(root, "TEST.BMP", File::O_CREAT | File::O_RDWR)){
    	file.rm();
    	file.close();
    }
    else{
    	Failure(7);
    }



//	PORTB |= (1<<PB5);


}


void loop() {


//	_delay_ms(1000);
//	if(rtc.Read()==false)
//	{
//		Failure(5);
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

//	while(true);

	uint8_t reg;
	uint8_t val;

	if (USART.available()>0)
	{

		uint8_t out=USART.read();
		switch (out){
		case 1:
//			PORTB |= (1<<PB5);
			break;
		case 2:
//			PORTB &= ~(1<<PB5);
			break;
		case '1':
			Can.Capture();
		    if(file.open(root, "TEST.BMP", File::O_CREAT | File::O_WRITE)==false)
		    	Failure(7);
			Fig_Count++;
			Can.Read_and_Save_Image();
			if (!file.close())
				Failure(8);
			break;
		case '2':
//			PORTB |= (1<<PB5);
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





