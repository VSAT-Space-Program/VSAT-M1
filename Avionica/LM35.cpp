/*
 *  LM35 for ATmega168/328
 *
 *  Pedro Araujo
 */
#define F_CPU 16000000L // tells the compiler at which speed AVRs clock is running (CPU frequency)
#define BAUD 9600UL     // specifies how fast data is transmitted over a serial line (9600 bps)

#include <avr/io.h>
#include <util/delay.h> // AVR standard libraries

#include <stdlib.h>       // C standard library (used for itoa and uart functions)
#include <util/setbaud.h> // given (F) and (BAUD) sets AVRs baudrate control registers to the correct values (smallest error rate).

// ================= ADC ==============
void adc_init()
{
  ADMUX = (1 << REFS0) | (1 << REFS1); // vref = 2.56V, 8-bit channel
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(uint8_t ch)
{
  ch &= 0b00001111;            //allow channels 0-15
  ADMUX = (ADMUX & 0xE0) | ch; // preserve bits 5,6,7
  ADCSRA |= (1 << ADSC);       // start adc
  while (ADCSRA & (1 << ADSC))
    ;         // wait untill finished
  return ADC; //return all 10 bits
}

// ================ UART ==================

void uart_init(void)
{
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  UCSR0B |= (1 << TXEN0);
}

void uart_putchar(char c)
{
  if (UCSR0A & (1 << UDRE0))
  {
    UDR0 = c;
  }
}

void uart_puts(char *s)
{
  while (*s)
  {
    uart_putchar((*s++));
  }
}

// ================== LM35 =================
int main(void)
{
  adc_init();  // Initialize ADC
  uart_init(); // Initialize 'Serial Monitor'
  while (1)
  {
    // LM35
    float mV = adc_read(0) * (1100.0 / 1024);
    float dgC = mV / 10; // dgC = degree in Celsius from LM35 datasheet --> 1mV = 10C

    char val[4];        // array to store print values
    itoa(dgC, val, 10); // Convert number in variable 'dgC' to a string and store in array 'val' in base '10'
    uart_puts(val);     // prints to monitor
    _delay_ms(1000);    // Acquisition time (>= 5us)
  }
}