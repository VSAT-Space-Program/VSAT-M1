/*
 * Baterry.h
 *
 *  Created on: 16 de mar de 2019
 *      Author: Eduardo Lacerda Campos
 */

#ifndef BATTERY_H_
#define BATTERY_H_
#include "Analog.h"

class BATTERY : public Analog_Digital {
public:
	BATTERY();
	virtual ~BATTERY();
	double Read_Voltage();
};

#endif /* BATTERY_H_ */
