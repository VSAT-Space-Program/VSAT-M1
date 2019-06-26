/*
 * Baterry.cpp
 *
 *  Created on: 16 de mar de 2019
 *      Author: educampos
 */

#include "Battery.h"

BATTERY::BATTERY() {
	// TODO Auto-generated constructor stub

}

BATTERY::~BATTERY() {
	// TODO Auto-generated destructor stub
}

double BATTERY::Read_Voltage(){

	//r_gnd -> resistor connected between knot 1 and the ground
	//r_vcc -> resistor connected between knot 1 and the Battery VCC
	//r_total = r_gnd+r_vcc
	//Cons = r_total / r_gnd

	//The resistors value must be defined where the maximum battery voltage
	// renders to 3.3V
	#define r_total 19.69  	//k ohm
	#define r_gnd 9.78 		//k ohm

	uint16_t Value= Read_Once(7);

	double current = (Value*3.3/1024.0)/r_gnd;

	Battery_Voltage = current*r_total;
	return Battery_Voltage;
}
