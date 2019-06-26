/*
 *
 */

#ifndef TIMERONE_cpp
#define TIMERONE_cpp

#include "TimerOne.h"

TimerOne Timer1;              // preinstatiate

ISR(TIMER1_OVF_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  Timer1.isrCallback();
}

ISR(TIMER1_CAPT_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  Timer1.IC_isrCallback();
}

ISR(TIMER1_COMPA_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  Timer1.isrCallback();
}
/*
 * Waveform Generation Mode Bit Description
 *
 * microseconds: period in microseconds
 *
 * mode 8: PWM, Phase and Frequency correct - top -> ICR1
 * mode 9: PWM, Phase and Frequency - top -> OCR1A
 *
 */
void TimerOne::initialize(long microseconds, uint8_t mode_)
{
	TCCR1A = 0;                 // clear control register A

	mode=mode_;
	TCCR1A |= (mode_ & 0b00000011);
	TCCR1B |= ((mode_ & 0b00001100)<<1);


	setPeriod(microseconds);
}

/*
 * Para uma CPU a 16Mz, o valor máximo é 8.388.608 microsegundos
 */
void TimerOne::setPeriod(long microseconds)		// AR modified for atomic access
{
  
  long cycles = (F_CPU / 2000000) * microseconds;                                // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
  if(cycles < RESOLUTION)              clockSelectBits = _BV(CS10);              // no prescale, full xtal
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11);              // prescale by /8
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11) | _BV(CS10);  // prescale by /64
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12);              // prescale by /256
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12) | _BV(CS10);  // prescale by /1024
//  else        cycles = RESOLUTION - 1, clockSelectBits = _BV(CS12) | _BV(CS10);  // request was out of bounds, set as maximum
  else        cycles = RESOLUTION , clockSelectBits = _BV(CS12) | _BV(CS10);  // request was out of bounds, set as maximum
  
  oldSREG = SREG;					//	Save AVR status register
  cli();							// Disable interrupts for 16 bit register access
  pwmPeriod = cycles -1;
  switch (mode)
  {
  case 8:
	  ICR1 = pwmPeriod; // ICR1 is TOP in phase & freq. correct pwm mode
	  break;
  case 4:
  case 9:
	  OCR1A = pwmPeriod;// OCR1A is TOP in phase & freq. correct pwm mode
	  break;
  }
  SREG = oldSREG;					//	Restor AVR status register and enable interruption
  
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));// reset clock select register
  TCCR1B |= clockSelectBits; 					//Define new timer clock
}

/*
 * duty must be between 0 and 1023
 */
void TimerOne::setPwmDuty(char pin, uint16_t duty)
{
  unsigned long dutyCycle = pwmPeriod;

  
  oldSREG = SREG;
  cli();
  switch (mode)
  {
  	  case 8:
  		  dutyCycle *= duty;
  		  dutyCycle >>= 10; // divided by 1024
		  if(pin == 1 || pin == 9)       OCR1A = dutyCycle;
		  else if(pin == 2 || pin == 10) OCR1B = dutyCycle;
		  break;
  	  case 9:
  		  OCR1A=duty*64;
  		  break;

  }

  SREG = oldSREG;
}

void TimerOne::setPwmDuty_Direct(char pin, uint16_t duty)
{

  if (duty > pwmPeriod)
	  duty = pwmPeriod;

  oldSREG = SREG;
  cli();
  switch (mode)
  {
  	  case 8:
		  if(pin == 1 || pin == 9)       OCR1A = duty;
		  else if(pin == 2 || pin == 10) OCR1B = duty;
		  break;
  	  case 9:
  		  OCR1A=duty;
  		  break;

  }

  SREG = oldSREG;
}

/*
 * pin: Arduino pin number. For time one must be 9 or 10
 * duty: initial value
 */
void TimerOne::pwm(char pin, int duty, long microseconds)  // expects duty cycle to be 10 bit (1024)
{
  if(microseconds > 0) setPeriod(microseconds);

  if (mode==9)
  {
	  if(pin == 9) {
		DDRB |= _BV(PORTB1);                                   // sets data direction register for pwm output pin
		TCCR1A |= _BV(COM1A0);                                 // activates the output pin
	  }
	  else if(pin == 10) {
		DDRB |= _BV(PORTB2);
		TCCR1A |= _BV(COM1B0);
	  }
  }
  else
  {
	  if(pin == 1 || pin == 9) {
		DDRB |= _BV(PORTB1);                                   // sets data direction register for pwm output pin
		TCCR1A |= _BV(COM1A1);                                 // activates the output pin
	  }
	  else if(pin == 2 || pin == 10) {
		DDRB |= _BV(PORTB2);
		TCCR1A |= _BV(COM1B1);
	  }
  }


  setPwmDuty(pin, duty);
  //resume();			// Lex - make sure the clock is running.  We don't want to restart the count, in case we are starting the second WGM
					// and the first one is in the middle of a cycle
}

void TimerOne::disablePwm(char pin)
{
  if(pin == 1 || pin == 9)       TCCR1A &= ~_BV(COM1A1);   // clear the bit that enables pwm on PB1
  else if(pin == 2 || pin == 10) TCCR1A &= ~_BV(COM1B1);   // clear the bit that enables pwm on PB2
}

void TimerOne::attachInterrupt(void (*isr)())
{
	isrCallback = isr;                                     // register the user's callback with the real ISR

	// sets the timer interrupt enable bit
	switch (mode)
	{
	case 4:
		TIMSK1 = _BV(OCIE1A);
		break;
	default:
		TIMSK1 = _BV(TOIE1);
		break;
	}

	// might be running with interrupts disabled (eg inside an ISR), so don't touch the global state
	//  sei();
	resume();
}

void TimerOne::detachInterrupt()
{
  TIMSK1 &= ~_BV(TOIE1);                                   // clears the timer overflow interrupt enable bit 
															// timer continues to count without calling the isr
}

void TimerOne::resume()				// AR suggested
{ 
  TCCR1B |= clockSelectBits;
}

void TimerOne::restart()		// Depricated - Public interface to start at zero - Lex 10/9/2011
{
	start();				
}

void TimerOne::start()	// AR addition, renamed by Lex to reflect it's actual role
{
  unsigned int tcnt1;
  
  TIMSK1 &= ~_BV(TOIE1);        // AR added 
  GTCCR |= _BV(PSRSYNC);   		// AR added - reset prescaler (NB: shared with all 16 bit timers);

  oldSREG = SREG;				// AR - save status register
  cli();						// AR - Disable interrupts
  TCNT1 = 0;                	
  SREG = oldSREG;          		// AR - Restore status register
	resume();
  do {	// Nothing -- wait until timer moved on from zero - otherwise get a phantom interrupt
	oldSREG = SREG;
	cli();
	tcnt1 = TCNT1;
	SREG = oldSREG;
  } while (tcnt1==0); 
 
//  TIFR1 = 0xff;              		// AR - Clear interrupt flags
//  TIMSK1 = _BV(TOIE1);              // sets the timer overflow interrupt enable bit
}

void TimerOne::stop()
{
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));          // clears all clock selects bits
}

unsigned long TimerOne::read()		//returns the value of the timer in microseconds
{									//remember! phase and freq correct mode counts up to then down again
  	unsigned long tmp;				// AR amended to hold more than 65536 (could be nearly double this)
  	unsigned int tcnt1;				// AR added

	oldSREG= SREG;
  	cli();							
  	tmp=TCNT1;    					
	SREG = oldSREG;

	char scale=0;
	switch (clockSelectBits)
	{
	case 1:// no prescalse
		scale=0;
		break;
	case 2:// x8 prescale
		scale=3;
		break;
	case 3:// x64
		scale=6;
		break;
	case 4:// x256
		scale=8;
		break;
	case 5:// x1024
		scale=10;
		break;
	}
	
	do {	// Nothing -- max delay here is ~1023 cycles.  AR modified
		oldSREG = SREG;
		cli();
		tcnt1 = TCNT1;
		SREG = oldSREG;
	} while (tcnt1==tmp); //if the timer has not ticked yet

	//if we are counting down add the top value to how far we have counted down
	tmp = (  (tcnt1>tmp) ? (tmp) : (long)(ICR1-tcnt1)+(long)ICR1  );		// AR amended to add casts and reuse previous TCNT1
	return ((tmp*1000L)/(F_CPU /1000L))<<scale;
}
/*
 * ICP_trigger = (Timer/Counter1 Input Capture Input)
 * True -> Analog Comparator Input Capture Enable
 * False -> PB0 -> ICP1
 */
void TimerOne::Input_Capture(bool ICP_trigger)
{
	//Bit 2 – ACIC: Analog Comparator Input Capture Enable
	//When written logic one, this bit enables the input capture function in Timer/Counter1 to be triggered by the
	//Analog Comparator.
	//When written logic zero is trigged by PB0 -> ICP1 (Timer/Counter1 Input Capture Input)

	if (ICP_trigger)
	{
		ACSR |= _BV(ACIC);
	}
	else
	{
		ACSR &= !_BV(ACIC);
	}
	/*
	 *  When the
	 * ICES1 bit is written to zero, a falling (negative) edge is used as trigger,
	 * and when the ICES1 bit is written to one, a rising (positive) edge will
	 * trigger the capture.
	 *
	 */
	TCCR1B |= _BV(ICES1);

}

void TimerOne::Input_Capture_attachInterrupt(void (*isr)())
{
	IC_isrCallback = isr;

	//Bit 5 – ICIE1: Timer/Counter1, Input Capture Interrupt Enable
	TIMSK1 |= _BV(ICIE1);
}


uint16_t TimerOne::Input_Capture_Read()
{
	return ICR1;
}

#endif
