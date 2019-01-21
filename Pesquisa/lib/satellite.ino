/*
  Arduino VSAT Lib

  Principal function:


  Created : January 21, 2019
  Author: Leticia Coelho 
*/



#include "VSAT.h"

VSAT vsat;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
     ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop() {

     int i = 1;
     vsat.TX_Lora(i);
}
