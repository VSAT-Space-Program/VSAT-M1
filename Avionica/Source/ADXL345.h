/* accelerometer digital sensor
 * ADXL345.hpp
 *
 *  Created on: 22 de out de 2018
 *      Author: educampos
 */

#ifndef ADXL345_H_
#define ADXL345_H_


#include "Error.h"
#include "i2c.h"
#include "Timer.h"

#define EARTH_GRAVITY_MS2  9.80665
#define SCALE_MULTIPLIER   0.0039

#define DATA_FORMAT        0x31
#define BW_RATE            0x2C
#define POWER_CTL          0x2D

#define BW_RATE_1600HZ  0x0F //output rate 3200Hz
#define BW_RATE_800HZ   0x0E //output rate 1600Hz
#define BW_RATE_400HZ   0x0D //output rate 800Hz
#define BW_RATE_200HZ   0x0C //output rate 400Hz
#define BW_RATE_100HZ   0x0B //output rate 200Hz
#define BW_RATE_50HZ    0x0A //output rate 100Hz
#define BW_RATE_25HZ    0x09 //output rate 50Hz
#define BW_RATE_12_5HZ  0x08 //output rate 25Hz
#define BW_RATE_6_25HZ  0x07 //output rate 12.5Hz
#define BW_RATE_3_125HZ 0x06 //output rate 6.25Hz

#define AXES_REG        0x32
#define MEASURE 		0x08


//DATA_FORMAT bits
#define FULL_RES        0x08
#define RANGE_2G        0x00
#define RANGE_4G        0x01
#define RANGE_8G        0x02
#define RANGE_16G       0x03
#define SELF_TEST       0x80
#define SPI_4WIRE       0x40
#define INT_INVERT      0x20


#define ADRRESS      	0x53

class ADXL345{
    public:

	bool Status=0;
	double data[3];
	double &x=data[0];
	double &y=data[1];
	double &z=data[2];
	double temp;
	I2C *Dev;

	double bias[3] = {-10.5000 ,  13.5000 ,  -2.0000};


	void Setup(Timer &timer , I2C *Dev_);
	bool Read(void);

	private:

	uint8_t buf[8];


};


//void setup_ADXL345(Timer &timer);



#endif /* ADXL345_HPP_ */
