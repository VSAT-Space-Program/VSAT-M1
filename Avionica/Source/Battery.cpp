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

	//r_gnd -> resistor connected to the ground
	//r_vcc -> resistor connected to the vcc
	//r_total = r_gnd+r_vcc
	//Cons = r_total / r_gnd

//	float r_total =19.69;
//	float r_gnd =9.78;

	//to reduce the amount of math during the calculation, the constant resistor values was used
	return (Read_Once(7)*(19.69/9.78)*5.03)/1024.0;
}
