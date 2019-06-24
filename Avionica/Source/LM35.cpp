 /*
    -> LM35.cpp                                                 0v -> 0                         Tensão Pin -> (Valor_pin)*(5.0/1023)
    Data -> 20/06/2019                                          5v -> 1023                      Temp = Tensão Pin / 10mV
    Autor -> João Brito                                         Celsius -> 10mV/ºCelsius        Temp = (Valor_pin*(5.0/1023))/0.01
                                                                10mV -> 0.01

 */
#include <LM35.h>

LM35::LM35(const int pin){

  pin_LM35 = pin;                             //Construtor
}
LM35::~LM35(){
                                              // Desconstrutor
}

float LM35::getC(){

    analogRead(pin_LM35);                               // Lê Pino Analógico.
    return analogRead((pin_LM35)*(5.0/1023.0))/0.01;   // Retorna Temperatura em Graus Celsius.

}
float LM35::getF(){

    analogRead(pin_LM35);                           // Lê Pino Analógico.
    return analogRead(pin_LM35)*300.0/341.0+32.0;   // Retorna Temperatura em Graus Fahrenheit.
}
