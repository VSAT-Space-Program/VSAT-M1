
#ifndef ANALOG_cpp
#define ANALOG_cpp

#include "Analog.h"

Analog_Digital Analog2Digital;

ISR(ADC_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
	Analog2Digital.isrCallback();
}


void Analog_Digital::initialize()
{

	//Prescaler Selection = CPU_FREQ/4
	ADCSRA= 0b00000010;

	/* Enable the ADC */
	ADCSRA |= _BV(ADEN);

	/*
	 *    Bit 7:6 – REFS[1:0]: Reference Selection Bits			=AVCC with external capacitor at AREF pin,
	 *    Bit 5 – ADLAR: ADC Left Adjust Result					=0
	 *	  Bits 3:0 – MUX[3:0]: Analog Channel Selection Bits  	=0
	 *
	 */
	ADMUX   |=  _BV(REFS0);

}


uint16_t Analog_Digital::Read_Once(uint8_t adcx) {
	/* adcx is the analog pin we want to use.  ADMUX's first few bits are
	 * the binary representations of the numbers of the pins so we can
	 * just 'OR' the pin's number with ADMUX to select that pin.
	 * We first zero the four bits by setting ADMUX equal to its higher
	 * four bits. */
	ADMUX	&= 0xf0;
	ADMUX	|=	adcx & 0x07;

	/* This starts the conversion. */
	ADCSRA |= _BV(ADSC);

	/* This is an idle loop that just wait around until the conversion
	 * is finished.  It constantly checks ADCSRA's ADSC bit, which we just
	 * set above, to see if it is still set.  This bit is automatically
	 * reset (zeroed) when the conversion is ready so if we do this in
	 * a loop the loop will just go until the conversion is ready. */
	while ( (ADCSRA & _BV(ADSC)) );

	/* Finally, we return the converted value to the calling function. */
	return ADC;
}

void Analog_Digital::attachInterrupt(void (*isr)(),uint8_t ADTS)
{

	// Bit 2:0 – ADTS[2:0]: ADC Auto Trigger Source Selections
	ADCSRB |= ADTS;

	// register the user's callback with the real ISR
	isrCallback = isr;

	/*
	 * ADSC ->This starts the conversion.
	 * ADATE-> ADC Auto Trigger Enable
	 * ADIE -> ADC Interrupt Enable
	 *
	 */
	ADCSRA |= _BV(ADSC) | _BV(ADIE) | _BV(ADATE);

}

#endif
