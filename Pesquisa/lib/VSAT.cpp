/*

  Arduino VSAT Lib

  Functions created in this library:

  "TX_Lora"     ->  LoraWan Transmiter
  "RX_Lora"     ->  LoraWan Receptor

  Created : January 21, 2019
  Author: Leticia Coelho 
  
*/

#include <VSAT.h>

int VSAT::TX_Lora(int data_to_transmit) /* LoraWan Transmiter */
{
  Serial.print("... TX_Lora ...");
}
       
int VSAT::RX_Lora() /* LoraWan Receptor */
{
  Serial.print("... RX_Lora ...");
}
