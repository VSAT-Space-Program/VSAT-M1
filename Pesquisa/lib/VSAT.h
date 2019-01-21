/*
  Arduino VSAT Lib

  Functions created in this library:

  "TX_Lora"     ->  LoraWan Transmiter
  "RX_Lora"     ->  LoraWan Receptor

  Created : January 21, 2019
  Author: Leticia Coelho 
*/

#ifndef VSAT_h
#define VSAT_h

#include "Arduino.h"

class VSAT
{
   public:
	int someThing; 
	VSAT VSAT();
	int TX_Lora(int data_to_transmit); /* LoraWan Transmiter */
        int RX_Lora(); /* LoraWan Receptor */
       	
};

#endif
