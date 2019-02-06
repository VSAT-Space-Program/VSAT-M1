#include "AnalogReader.h"

#include <Arduino.h>

AnalogReader::AnalogReader(const int pin) : m_pin(pin) {}

/**
 * le valor analogico da placa ADC
 */
int AnalogReader::read()
{
	return analogRead(m_pin);
}
