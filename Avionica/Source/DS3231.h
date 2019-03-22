	/*
	* DS3221.h
	*
	*  Created on: 22 de fev de 2019
	*      Author: joaobrito
	*/
	
	#ifndef DS3231_H_
	#define DS3231_H_
	
	#include <Wire.h>
	
	#include "DateTime.h"
	
	#define DS3231_ADDRESS  							0x68
	#define DS3231_CONTROL  							0x0E
	#define DS3231_STATUSREG 							0x0F
	#define DS3231_SECOND_REGISTER    					0x00
	#define DS3231_TEMPERATURE_REGISTER_LOW_BYTE    	0x12
	#define DS3231_TEMPERATURE_REGISTER_HIGH_BYTE    	0x11
	
	#define CONV_TEMPERATURE_FORCE_BIT 			0x20
	#define CONV_TEMPERATURE_FORCE_BUSY_BIT 	0x08
	
	
	class DS3231 {
	public:
	DS3231();
	virtual ~DS3231();
	bool Initialize(TwoWire* Wire);
	bool Read();
	bool Read_Temperature();
	double Get_Temperature();
	bool Force_Temperature_Update();
	bool Adjust_Time(const DateTime& dt);
	byte readControlByte(bool which);
	
	
	void enableOscillator(bool TF, bool battery, byte frequency);
	// turns oscillator on or off. True is on, false is off.
	// if battery is true, turns on even for battery-only operation,
	// otherwise turns off if Vcc is off.
	// frequency must be 0, 1, 2, or 3.
	// 0 = 1 Hz
	// 1 = 1.024 kHz
	// 2 = 4.096 kHz
	// 3 = 8.192 kHz (Default if frequency byte is out of range);
	void enable32kHz(bool TF);
	// Turns the 32kHz output pin on (true); or off (false).
	bool oscillatorCheck();;
	// Checks the status of the OSF (Oscillator Stop Flag);.
	// If this returns false, then the clock is probably not
	// giving you the correct time.
	// The OSF is cleared by function setSecond();.
	
	
	
	uint8_t data[6];
	uint8_t &ss=data[0];
	uint8_t &mm=data[1];
	uint8_t &hh=data[2];
	uint8_t &y=data[3];
	uint8_t &d=data[4];
	uint8_t &m=data[5];
	
	private:
	byte readControlByte(bool which);
	// Read selected control byte: (0); reads 0x0e, (1) reads 0x0f
	void writeControlByte(byte control, bool which);
	// Write the selected control byte.
	// which == false -> 0x0e, true->0x0f.
	
	TwoWire* Wire;
	
	uint8_t Temp;
	uint8_t Temp_float;
	};
	
	
	#endif /* DS3231_H_ */
