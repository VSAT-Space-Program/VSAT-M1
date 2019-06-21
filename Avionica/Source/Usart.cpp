/**
 * Implement the Usart class.
 * It allows communication in Arduino UARTn/USARTn ports, n = {0, 1, 2, 3}
 *
 * @file Usart.cpp
 * @version 1.0 
 * @created 29.01.2015 12:27:39
 * @author: Mircea Diaconescu
 */ 

#include "Usart.h"
#include <new.h>
#include <math.h>

/************************************************************************/
/* @constructor                                                         */
/* Create a Usart object                                                */
/************************************************************************/
Usart::Usart() {
  this->portNo = 0;

  this->udr = &UDR0;
  this->ucsra = &UCSR0A;
  this->ucsrb = &UCSR0B;
  this->ucsrc = &UCSR0C;
  this->ubrrh = &UBRR0H;
  this->ubrrl = &UBRR0L;
  this->txen = TXEN0;
  this->rxen = RXEN0;
  this->udre = UDRE0;
  this->u2x = U2X0;
  this->rxcie = RXCIE0;
  this->usbs = USBS0;
  this->upm0 = UPM00;
  this->upm1 = UPM01;
  this->ucsz0 = UCSZ00;
  this->ucsz1 = UCSZ01;
  this->ucsz2 = UCSZ02;
  this->umsel0 = UMSEL00;
  this->umsel1 = UMSEL01;

  /** default communication settings **/  
  // use no parity
  this->setParity( UsartParityEL::NONE);
  // use one stop bit
  this->setStopBit( UsartStopBitEL::ONE);
  // frame (data) length = 8 bits
  this->setFrameLength( UsartFrameLengthEL::EIGHT_BITS);

  this->rxBuffStart = NULL;
  this->rxDataStart = 0;
  this->rxBuffEnd = NULL;
  this->rxDataEnd = 0;
  this->rxMaxLength = 0;
  this->rxDataTStart = NULL;
  this->baud=0;
  this->rxLength=0;

};

/************************************************************************/
/* @destructor                                                          */
/* Destroy the Usart object by freeing up the allocated memory          */
/************************************************************************/
Usart::~Usart() {
  delete this->rxBuffStart; 
};

/************************************************************************/
/* @method                                                              */
/* Set UART/USART frame length (5 to 9 bits)                            */
/* @param frameLength                                                   */
/*          the frame length - values from UsartFrameLengthEL.xxx       */
/************************************************************************/
void Usart::setFrameLength( UsartFrameLengthEL frameLength) {
  switch ( frameLength) {
    case UsartFrameLengthEL::FIVE_BITS: 
      // set data transmission mode: 5-bit (UCSZn2 = 0; UCSZn1 = 0; UCSZn0 = 0;)
      *(this->ucsrc) &= ~(1 << this->ucsz0) | ~(1 << this->ucsz1); 
      // UCSZn2 bit is located in UCSRnB registry!
      *(this->ucsrb) &= ~(1 << this->ucsz2);      
    break;
    case UsartFrameLengthEL::SIX_BITS: 
      // set data transmission mode: 6-bit (UCSZn2 = 0; UCSZn1 = 0; UCSZn0 = 1;)
      *(this->ucsrc) |= (1 << this->ucsz0);
      *(this->ucsrc) &= ~(1 << this->ucsz1);  
      // UCSZn2 bit is located in UCSRnB registry!
      *(this->ucsrb) &= ~(1 << this->ucsz2); 
    break;
    case UsartFrameLengthEL::SEVEN_BITS: 
      // set data transmission mode: 7-bit (UCSZn2 = 0; UCSZn1 = 1; UCSZn0 = 0;)
      *(this->ucsrc) &= ~(1 << this->ucsz0);
      *(this->ucsrc) |= (1 << this->ucsz1);  
      // UCSZn2 bit is located in UCSRnB registry!
      *(this->ucsrb) &= ~(1 << this->ucsz2); 
    break;
    case UsartFrameLengthEL::EIGHT_BITS: 
      // set data transmission mode: 8-bit (UCSZn2 = 0; UCSZn1 = 1; UCSZn0 = 1;)
      *(this->ucsrc) |= (1 << this->ucsz0) | (1 << this->ucsz1); 
      // UCSZn2 bit is located in UCSRnB registry!
      *(this->ucsrb) &= ~(1 << this->ucsz2);      
    break;
    case UsartFrameLengthEL::NINE_BITS: 
      // set data transmission mode: 9-bit (UCSZn2 = 1; UCSZn1 = 1; UCSZn0 = 1;)
      *(this->ucsrc) |= (1 << this->ucsz0) | (1 << this->ucsz1);  
      // UCSZn2 bit is located in UCSRnB registry!
      *(this->ucsrb) |= (1 << this->ucsz2); 
    break;
  }  
};

/************************************************************************/
/* @method                                                              */
/* Get UART/USART frame length                                          */
/* @return one of UsartFrameLengthEL.xxx                                     */
/************************************************************************/
UsartFrameLengthEL Usart::getFrameLength() {
  // UCSZn0 and UCSZn1 bits are found in the UCSRnC register
  uint8_t ucsz0 = (*(this->ucsrc) >> this->ucsz0) & 1;
  uint8_t ucsz1 = (*(this->ucsrc) >> this->ucsz1) & 1;
  // UCSZn2 bit is found in the UCSRnB register
  uint8_t ucsz2 = (*(this->ucsrb) >> this->ucsz2) & 1;
  if ( ucsz2 == 0 && ucsz1 == 0 && ucsz0 == 0) {
    return UsartFrameLengthEL::FIVE_BITS;
  } else if ( ucsz2 == 0 && ucsz1 == 0 && ucsz0 == 1) {
    return UsartFrameLengthEL::SIX_BITS;
  } else if ( ucsz2 == 0 && ucsz1 == 1 && ucsz0 == 0) {
    return UsartFrameLengthEL::SEVEN_BITS;
  } else if ( ucsz2 == 0 && ucsz1 == 1 && ucsz0 == 1) {
    return UsartFrameLengthEL::EIGHT_BITS;
  } else if ( ucsz2 == 1 && ucsz1 == 1 && ucsz0 == 1) {
    return UsartFrameLengthEL::NINE_BITS;
  }
};

/************************************************************************/
/* @method                                                              */
/* Set UART/USART transmission parity                                   */
/* @param parity                                                        */
/*          the parity - values from UsartParityEL.xxx                  */
/************************************************************************/
void Usart::setParity( UsartParityEL parity) {
  switch ( parity) {
    case UsartParityEL::NONE:
      // set data transmission mode: no parity ( UPMn0 = 0; UPMn1 = 0;)
      *(this->ucsrc) &= ~(1 << this->upm0) | ~(1 << this->upm1);
    break;
    case UsartParityEL::EVEN: 
      // set data transmission mode: even ( UPMn0 = 0; UPMn1 = 1;)
      *(this->ucsrc) &= ~(1 << this->upm0);
      *(this->ucsrc) |= (1 << this->upm1);
    break;
    case UsartParityEL::ODD:
      // set data transmission mode: odd ( UPMn0 = 1; UPMn1 = 1;)
      *(this->ucsrc) |= (1 << this->upm0) | (1 << this->upm1);
    break;
  }
};

/************************************************************************/
/* @method                                                              */
/* Get UART/USART transmission parity                                   */
/* @return one of UsartParityEL.xxx                                     */
/************************************************************************/
UsartParityEL Usart::getParity() {
  uint8_t upm0 = (*(this->ucsrc) >> this->upm0) & 1;
  uint8_t upm1 = (*(this->ucsrc) >> this->upm1) & 1;
  if ( upm0 == 0 && upm1 == 0) {
    return UsartParityEL::NONE;
  } else if ( upm0 == 0 && upm1 == 1) {
    return UsartParityEL::EVEN;
  } if ( upm0 == 1 && upm1 == 0) {
    return UsartParityEL::ODD;
  } 
  return UsartParityEL::ERRO;
}

/************************************************************************/
/* @method                                                              */
/* Set UART/USART transmission stop bit                                 */
/* @param stopBit                                                       */
/*          the stop bit - values from UsartStopBitEL.xxx               */
/************************************************************************/
void Usart::setStopBit( UsartStopBitEL stopBit) {
  switch ( stopBit) {
    // one stop bit
    case UsartStopBitEL::TWO:
      *(this->ucsrc) &= ~( 1 << this->usbs);
    break; 
    // two stop bits
    case UsartStopBitEL::ONE:
      *(this->ucsrc) |= ( 1 << this->usbs);
    break;
  }
};

/************************************************************************/
/* @method                                                              */
/* Get UART/USART transmission stop bit                                 */
/* @return one of UsartStopBitEL.xxx                                    */
/************************************************************************/
UsartStopBitEL Usart::getStopBit() {
  uint8_t usbsBit = (*(this->ucsrc) >> this->usbs) & 1;
  switch ( usbsBit) {
    case 0:
      return UsartStopBitEL::ONE;
    break;
    case 1:
      return UsartStopBitEL::TWO;
    break;
  }
};

/************************************************************************/
/* @method                                                              */
/* Check if any data is stored in the RX buffer                         */
/* @return the number of bytes available in the RX buffer (0 if empty)  */
/************************************************************************/
uint16_t Usart::available() {
	cli();
	uint16_t out = this->rxLength;
	sei();
  return out;
};

/************************************************************************/
/* @method                                                              */
/* Clear the RX buffer - reset buffer pointers to their initial state   */
/* @return true so it can be used in logical expressions with ease      */
/************************************************************************/
bool Usart::clear() {
  bool txEnabled =  (*(this->ucsrb) & (1 << this->txen)) > 0;
  // disable USART data receiving until clearing the buffer
  if ( txEnabled) {
    *(this->ucsrb) &= ~(1 << this->txen);
  }
  this->rxDataStart = this->rxBuffStart;
  this->rxDataTStart = this->rxBuffStart;
  this->rxDataEnd = this->rxBuffStart;
  this->rxLength = 0;
  this->rxLostBytesNr = 0;
  // if TX was enabled when entering this method
  // then enable it back after cleared the buffer
  if ( txEnabled) {
    *(this->ucsrb) |= (1 << this->txen);
  }
  return true;
};

/************************************************************************/
/* @method                                                              */
/* Get the maximum length of the RX buffer                              */
/* @return the maximum number of bytes stored by the RX buffer          */
/************************************************************************/
uint16_t Usart::getMaxLength() {
  return this->rxMaxLength;
};

/************************************************************************/
/* @method                                                              */
/* Get the number of bytes which were lost because TX buffer overrun    */
/* The number of bytes are computed from the last time when the buffer  */
/* was cleared (by calling the 'clear' method.                          */
/* NOTE: the value is represented as a 16 bits unsigned integer!        */
/* @return the number of RX bytes lost because buffer overrun           */
/************************************************************************/
uint16_t Usart::getRxLostBytesNr() {
  return this->rxLostBytesNr;
};

/************************************************************************/
/* @method                                                              */
/* Start USART communication                                            */
/* NOTE: calling this produces also the RX buffer memory allocation     */
/* @param baud                                                          */
/*          the baud rate (in bps, e.g. 9600)                           */
/* @param rxBuffMaxLen                                                  */
/*          the maximum size (number of bytes) stored by the RX buffer  */
/* @param mode                                                          */
/*           the usart mode (one of UsartModeEL.xxx)                    */
/************************************************************************/
void Usart::begin( const uint32_t baud, uint16_t rxBuffMaxLen, UsartModeEL mode) {
  uint16_t bRate = 0;
  uint8_t bMode = 0;
  // initialize the buffer
  this->rxMaxLength = rxBuffMaxLen;
  this->rxBuffStart = (uint8_t*)malloc( this->rxMaxLength);
  this->rxBuffEnd = this->rxBuffStart + this->rxMaxLength;
  this->rxDataStart = this->rxBuffStart;
  this->rxDataTStart = this->rxBuffStart;
  this->rxDataEnd = this->rxBuffStart;
  this->rxLength = 0;
  this->rxLostBytesNr = 0;

  // store baud rate
  this->baud = baud;
  // compute baud rate value for UBRR register
  switch ( mode) {
    case UsartModeEL::SYNC_MASTER:
      bRate = ( F_CPU / 2 / baud - 1) / 2;
      bMode = 0;
      // UMSELn0 = 1 and UMSELn1 = 0 for synchronous mode 
      *(this->ucsrc) |= (1 << this->umsel0);
      *(this->ucsrc) &= ~(1 << this->umsel1);
    break;
    case UsartModeEL::ASYNC_DOUBLE_SPEED:
      bRate = ( F_CPU / 4 / baud - 1) / 2;
      bMode = 1;
      // UMSELn0 = 0 and UMSELn1 = 0 for asynchronous mode 
      *(this->ucsrc) &= ~(1 << this->umsel0) | ~(1 << this->umsel1);
    break;
    case UsartModeEL::ASYNC_NORMAL:
      bRate = ( F_CPU / 8 / baud - 1) / 2;
      bMode = 0;
      // UMSELn0 = 0 and UMSELn1 = 0 for asynchronous mode 
      *(this->ucsrc) &= ~(1 << this->umsel0) | ~(1 << this->umsel1);
    break;
  }
  // Set USART transmission mode:
  // U2Xn = 0 for asynchronous, normal speed or synchronous 
  // U2Xn = 1 for asynchronous, double speed
  *(this->ucsra) = ( bMode << this->u2x);
  // set the USART High register byte
  *(this->ubrrh) = ( bRate >> 8);
  // set the USART Low register byte
  *(this->ubrrl) =  bRate;
  // enable USART transmission
  *(this->ucsrb) |= (1 << this->txen);
  // enable USART reception & USARTn_RX_vect interrupt
  *(this->ucsrb) |= (1 << this->rxen) | (1 << this->rxcie);
  // set asynchronous USART mode (UMSEL00 = 0; UMSEL01 = 0;)
  /*UCSR0C &= ~(1 << UMSEL00) | ~(1 << UMSEL01);
 
 */
  // enable interrupts
  sei();
};

/************************************************************************/
/* @method                                                              */
/* Read the next byte from the RX buffer                                */
/* NOTE: call this method only after checking that the buffer is not    */
/*       empty, i.e., by calling the 'available' method, otherwise the  */
/*       returned value will be 0!                                      */
/* @param removeReadByte                                                */
/*          flag allowing to specify if the byte is deleted from buffer */
/* @return the next byte from the buffer                                */
/************************************************************************/
uint8_t Usart::read( bool removeReadByte) {
	uint8_t data = 0;

	// return 0 if the buffer is empty
	if ( available() == 0) {
	return 0;
	}

	data = *(this->rxDataStart);

	if ( removeReadByte) {
		this->rxDataStart++;
		if ( this->rxDataStart == this->rxBuffEnd) {
			this->rxDataStart = this->rxBuffStart;
		}
		cli();
		this->rxLength--;
		sei();
	}

	return data;
};

/************************************************************************/
/* @method                                                              */
/* Write a new byte in the UDRn register, for being transmitted         */
/* @param ub                                                            */
/*          the unsigned byte to write                                  */
/************************************************************************/
void Usart::writeByte( uint8_t ub) {
  // Wait for URD0 register to be ready for the next data byte.
  // The URDE0 bit from the UCSR0A registry is set when USART 
  // data register is empty and new data can be transmitted
 	loop_until_bit_is_set( *(this->ucsra), this->udre);
  // write the next data byte
  *(this->udr) = ub;
};

/************************************************************************/
/* @method                                                              */
/* Write a string to USART where the string source is PROGMEM           */
/* @param pmData                                                        */
/*          the PROGMEM source                                          */
/************************************************************************/
void Usart::writeFromPM( const char pmData[]) {
  char c = 0;
  while ( 0 != ( c = pgm_read_byte(pmData++))) {
    this->writeByte( c);
  } 
};
  
/************************************************************************/
/* @method                                                              */
/* Write a set of unsigned bytes value to USART output.                 */
/* @param data                                                          */
/*          the pointer referencing the set of bytes to write           */
/* @param dataLen                                                       */
/*          the length (number of bytes) of the data to be transmitted  */
/************************************************************************/
void Usart::writeBytes( uint8_t* data, const uint16_t dataLen) {
  for ( uint16_t i = 0; i < dataLen; i++) {
    this->writeByte( *(data++));
  }
};

/************************************************************************/
/* @method                                                              */
/* Write a string ( '\0' terminated char array/pointer) to USART.       */
/* Maximum allowed length for the string depends on the memory size!    */
/* @param data                                                          */
/*          the pointer referencing the string to write                 */
/************************************************************************/
void Usart::write( const char* data) {
  for ( const char* s = data; *s; ++s) {
    this->writeByte( (uint8_t)*s);
  }
};

/************************************************************************/
/* @method                                                              */
/* Write a signed long number to USART.                                 */
/* @param num                                                           */
/*          the number to write                                         */
/************************************************************************/
void Usart::write( int32_t num) {
  if ( num < 0) {
    this->writeByte( '-');
    num = -num;
  }
  this->write( (uint32_t) num);
}

/************************************************************************/
/* @method                                                              */
/* Write an unsigned long number to USART.                              */
/* @param num                                                           */
/*          the number to write                                         */
/************************************************************************/
void Usart::write( uint32_t num) {
  // max digits number for signed/unsigned long is 10
  char digits[10];
  uint8_t len = 0;
  do {
    digits[len] = '0' + ( num % 10);
    len++;
  } while ( num /= 10);
  while ( len > 0) {
     this->writeByte( digits[--len]);  
  }
}

/************************************************************************/
/* @method                                                              */
/* Write a signed integer number to USART.                              */
/* @param num                                                           */
/*          the number to write                                         */
/************************************************************************/
void Usart::write(int16_t num) {
  this->write((int32_t) num);
}

/************************************************************************/
/* @method                                                              */
/* Write an unsigned integer number to USART.                           */
/* @param num                                                           */
/*          the number to write                                         */
/************************************************************************/
void Usart::write(uint16_t num) {
  this->write((uint32_t) num);
}

/************************************************************************/
/* @method                                                              */
/* Write an double number to USART.                           */
/* @param num                                                           */
/*          the number to write                                         */
/************************************************************************/

void Usart::write(double number, uint8_t digits)
{

  if (isnan(number)) return write("nan");
  if (isinf(number)) return write("inf");
  if (number > 4294967040.0) return write ("ovf");  // constant determined empirically
  if (number <-4294967040.0) return write ("ovf");  // constant determined empirically

  // Handle negative numbers
  if (number < 0.0)
  {
     write("-");
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  write((uint32_t)int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
    write(".");
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    unsigned int toPrint = (unsigned int)(remainder);
    write(toPrint);
    remainder -= toPrint;
  }

}

/************************************************************************/
/* @method                                                              */
/* Find data in the USART buffer                                        */
/* This method clears the USART buffer up to, and including the data,   */
/* if the data is found, otherwise clears all the data from the buffer  */ 
/* NOTE: the maximum data length is 255!                                */
/*                                                                      */
/* @param data                                                          */
/*          the data to search for in the buffer                        */
/* @return true if data was found, false otherwise                      */
/************************************************************************/
bool Usart::find( const char* data, bool removeReadByte) {
  uint8_t pos = 0, len = 0, avl = 0;
  const char* s;
  for ( s = data; *s; ++s, len++); 
  // null search data..
  if ( *data == 0) {
    return false;
  }
  // searching the TX buffer as long as it contains data
  avl = this->available();
  this->rxDataTStart = this->rxDataStart;
  while ( avl > 0) {
    // char/byte match on current position
    if ( *(data + pos) == this->read( removeReadByte)) {
      pos++;
    } 
    // no match for current position
    else {
      pos = 0;
    }
    // full data match!
    if ( pos == len) {
      return true;
    }
    avl--;
  }
  // no match if this point was reached
  return false;
};

/************************************************************************/
/* @method                                                              */
/* Find data in the USART buffer - the data to find is extracted        */
/* from PROGMEM ( program memory area)                                  */
/* This method clears the USART buffer up to, and including the data,   */
/* if the data is found, otherwise clears all the data from the buffer  */
/* NOTE: the maximum data length is 255!                                */
/*                                                                      */
/* @param pmData                                                        */
/*          the PROGMEM data to search for in the buffer                */
/* @return true if data was found, false otherwise                      */
/************************************************************************/
bool Usart::findFromPM( const char pmData[], bool removeReadByte) {
  char c = 0;
  const char* data = pmData;
  char* ramData = nullptr;
  uint8_t len = 0;
  bool result = false;
  // read the data from PROGMEM for length computation
  while ( 0 != ( c = pgm_read_byte(data++))) {
    len++;
  }  
  // load the data from PROGMEM and store it in RAM
  ramData = (char*)malloc( len + 1);
  data = pmData;
  while ( 0 != ( c = pgm_read_byte(data++))) {
    *(ramData) = c;
    ramData++;
  } 
  *(ramData) = '\0';
  ramData -= len;
  result = this->find( ramData, removeReadByte);
  free( ramData); 
  return result;
};

/************************************************************************/
/* @method                                                              */
/* Define the actions performed by the interrupt. This method is used   */
/* with the USARTn_RX_vector to perform a USART buffered reading.       */
/* @param udr                                                           */
/*          the UDRn register                                           */
/* @param usart                                                         */
/*          the Usart instance used with the interrupt (USARTn)         */
/************************************************************************/
void rxVector( uint8_t udr, Usart* usart) {
//  *(usart->rxDataEnd) = udr;
//  usart->rxDataEnd++;
//  if ( usart->rxDataEnd == usart->rxBuffEnd) {
//    usart->rxDataEnd = usart->rxBuffStart;
//  }
//  if ( usart->rxLength == usart->rxMaxLength) {
//    usart->rxLostBytesNr++;
//    if ( ( usart->rxDataStart + 1) == usart->rxBuffEnd) {
//      usart->rxDataStart = usart->rxBuffStart;
//    } else {
//      usart->rxDataStart++;
//    }
//  } else {
//    usart->rxLength++;
//  }
//

	if ( usart->rxLength < usart->rxMaxLength){

		*(usart->rxDataEnd) = udr;
		usart->rxDataEnd++;
		if ( usart->rxDataEnd == usart->rxBuffEnd)
		{
			usart->rxDataEnd = usart->rxBuffStart;
		}
		usart->rxLength++;
	}
	else
		usart->rxLostBytesNr++;

}

// define the global USART object(s)
// used to read/write via USART/Serial
// and set the USARTn_RX_vect interrupts
#if defined(HAS_USART) 
  Usart USART(0);

  ISR( USART_RX_vect) {
    rxVector( UDR0, &USART);
  };

#elif defined(HAS_USART0) 
  Usart USART;
  ISR(USART_RX_vect) {
    rxVector( UDR0, &USART);
  };
#endif



