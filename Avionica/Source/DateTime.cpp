/*
 * Datetime.cpp
 *
 *  Created on: 18 de jan de 2019
 *      Author: educampos
 */

#include "DateTime.h"

//DateTime::DateTime(){
//	// TODO Auto-generated constructor stub
//}

DateTime::DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{

    y = year;
    m = month;
    d = day;
    hh = hour;
    mm = min;
    ss = sec;

}


DateTime::~DateTime() {
	// TODO Auto-generated destructor stub
}

