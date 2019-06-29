/*
    -> LM35.h
    Data -> 20/06/2019
    Autor -> João Brito
    Revisor -> Eduardo Campos

*/

#ifndef _LM35_H_
#define _LM35_H_

#include <Analog.h>


class LM35 : public Analog_Digital{
	public:
		LM35(uint8_t pin, uint8_t GND);
		virtual ~LM35();

		float getC();     //  Temperatura em Celsius


	private:
		uint8_t pin_LM35;
		uint8_t GND_LM35;

};

#endif
