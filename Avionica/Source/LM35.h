/*
    -> LM35.h
    Data -> 20/06/2019
    Autor -> João Brito

*/

#ifndef _LM35_H_
#define _LM35_H_

#include <Analog.h>
#include <Arduino.h>


class LM35 : public Analog_Digital{
	public:
	    LM35(const int pin);
	    virtual ~LM35();

		float getC();     //  Temperatura em Celsius
        float getF(); //   Temperatura em Fahrenheit


  private:
    int pin_LM35
};

#endif
