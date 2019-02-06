#include "LM35.h"

/**
 *Constroi instancia do LM35
 * O output do sensor e linearmnete proporcional com a temperatura
 */
LM35::LM35(const int pin) : AnalogReader(pin)
{
  pinMode(pin, INPUT);
}


/**
 * medida media do sensor.<BR>
 * Retorna valores de temperatura entre -55 <-> 150 em Celsius
 */

float LM35::getTempC(byte calReadings)
{
  int val = 0;
  for (byte i =0 ; i < calReadings ; i++ )
    val += read();
  return (5.0 * val * 100.0) / 1024.0 / calReadings;
}

/**
 * Retorna valores de temperatura entre -67 <-> 302 em Fahrenheit.
 */
float LM35::getTempF(byte calReadings)
{
  return getTempC(calReadings) * 1.8 + 32;
}
