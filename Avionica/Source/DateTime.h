/*
 * Datetime.h
 *
 *  Created on: 18 de jan de 2019
 *      Author: educampos
 */

#ifndef DATETIME_H_
#define DATETIME_H_

#include <inttypes.h>

class DateTime {
public:
    DateTime(uint16_t year=0, uint8_t month=0, uint8_t day=0, uint8_t hour =0, uint8_t min =0, uint8_t sec =0);
	virtual ~DateTime();
    //DateTime (const DateTime& copy);
    uint16_t year() const       { return y; }
    uint8_t month() const       { return m; }
    uint8_t day() const         { return d; }
    uint8_t hour() const        { return hh; }
    uint8_t minute() const      { return mm; }
    uint8_t second() const      { return ss; }
protected:
    uint8_t y, m, d, hh, mm, ss;
};

#endif /* DATETIME_H_ */
