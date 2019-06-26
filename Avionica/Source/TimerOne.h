/*
 *
 *  See Google Code project http://code.google.com/p/arduino-timerone/ for latest
 */
#ifndef TIMERONE_h
#define TIMERONE_h


#ifdef COUNTERONE_h
#error Can not Use Same Timer1 for Different Functions!.
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include "stdint.h"

#define RESOLUTION 65536    // Timer1 is 16 bit

//Waveform Generation Mode Bit Description
/*
Normal  							0
PWM, Phase Correct, 8-bit  			1
PWM, Phase Correct, 9-bit   		2
PWM, Phase Correct, 10-bit  		3
CTC  								4
Fast PWM, 8-bit 					5
Fast PWM, 9-bit  					6
Fast PWM, 10-bit 					7
PWM, Phase and Frequency  Correct 	8
PWM, Phase and Frequency  Correct	9
PWM, Phase Correct  				10
PWM, Phase Correct  				11
CTC
*/
class TimerOne
{
  public:
  
    // properties
    unsigned int pwmPeriod;
    unsigned char clockSelectBits;
    uint8_t mode;
	char oldSREG;					// To hold Status Register while ints disabled

    // methods
    void initialize(long microseconds=1000000,uint8_t mode_= 9);
    void start();
    void stop();
    void restart();
	void resume();
	unsigned long read();
    void pwm(char pin, int duty, long microseconds=-1);
    void disablePwm(char pin);
    void attachInterrupt(void (*isr)());
    void detachInterrupt();
    void setPeriod(long microseconds);
    void setPwmDuty(char pin, uint16_t duty);
    void setPwmDuty_Direct(char pin, uint16_t duty);
    void (*isrCallback)();
    void (*IC_isrCallback)();
    void Input_Capture(bool ICP_trigger);
    uint16_t Input_Capture_Read();
    void Input_Capture_attachInterrupt(void (*isr)());
};

extern TimerOne Timer1;
#endif
