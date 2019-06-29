 /*
    -> LM35.cpp                                                 0v -> 0                         Tens�o Pin -> (Valor_pin)*(5.0/1023)
    Data -> 20/06/2019                                          5v -> 1023                      Temp = Tens�o Pin / 10mV
    Autor -> João Brito                                         Celsius -> 10mV/�Celsius        Temp = (Valor_pin*(5.0/1023))/0.01
   Revisor -> Eduardo Campos                                    10mV -> 0.01

 */
#include "LM35.h"

LM35::LM35(uint8_t pin, uint8_t GND){

  pin_LM35 = pin;
  GND_LM35 = GND;

}
LM35::~LM35(){
                                              // Desconstrutor
}

float LM35::getC(){

    uint16_t Value = Read_Once(pin_LM35);
    uint16_t GND_Value = Read_Once(GND_LM35);

    return (Value - GND_Value) *3.3/1023/0.01;

}

