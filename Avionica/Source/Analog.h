/*
 *  Interruptable Analog for ATmega168/328
 *
 *  Eduardo Lacerda Campos
 */
#ifndef ANALOG_h
#define ANALOG_h

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

// ADC Auto Trigger Source Selections
#define Free_Running_mode 0b00000000
#define Analog_Comparator 0b00000001
#define External_Interrupt_Request 0b00000010
#define Timer0_Compare_Match_A  0b00000011
#define Timer0_Overflow 0b00000100
#define Timer1_Overflow 0b00000110
//All modes arent defined



class Analog_Digital
{
  public:
  
    // properties
	char oldSREG;

    // methods
    void initialize();
    uint16_t Read_Once(uint8_t adcx);
//    void start();
//    void stop();
//    void restart();
    void attachInterrupt(void (*isr)(), uint8_t ADTS=Free_Running_mode);
//    void detachInterrupt();

    void (*isrCallback)();
};

extern Analog_Digital Analog2Digital;

#endif
