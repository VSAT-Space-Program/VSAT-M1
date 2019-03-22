	/*
	* DS3231.cpp
	*
	*  Created on: 22 de fev de 2019
	*      Author: joaobrito
	*/
	
	#include <DateTime.h>
	#include <DS3231.h>
	
	
	static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
	static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }
	
	
	DS3231::DS3231() {
	
	}
	
	DS3231::~DS3231() {
	
	}
	
	bool DS3231::Initialize(TwoWire* Wire)
	{
	this->Wire = Wire;
	
	return true;
	}
	
	void writeControlByte(byte control, bool which){ // Define Control bits
	//Write the selected control byte.
	// which=false -> 0x0e, true -> 0x0f.
	
	Wire.beginTransmission(DS3231_ADDRESS);
	if(which){
	Wire.write(0x0f);
	}else{
	Wire.write(0x0e);
	}
	Wire.write(control);
	Wire.endTransmission();
	}
	
	byte readControlByte(bool which) {
	// Read selected control byte
	// first byte (0) is 0x0e, second (1) is 0x0f
	Wire.beginTransmission(DS3231_ADDRESS);
	if (which) {
	// second control byte
	Wire.write(0x0f);
	} else {
	// first control byte
	Wire.write(0x0e);
	}
	Wire.endTransmission();
	Wire.requestFrom(DS3231_ADDRESS, 1);
	return Wire.read();
	}
	
	
	// enable oscillator
	void enableOscillator(bool TF, bool battery, byte frequency){
	// turns oscillator on or off. True is on, false is off.
	// if battery is true, turns on even for battery-only operation,
	// otherwise turns off if Vcc is off.
	// frequency must be 0, 1, 2, or 3.
	// 0 = 1 Hz
	// 1 = 1.024 kHz
	// 2 = 4.096 kHz
	// 3 = 8.192 kHz (Default if frequency byte is out of range)
	if (frequency > 3) frequency = 3;
	// read control byte in, but zero out current state of RS2 and RS1.
	byte temp_buffer = readControlByte(0) & 0xE7;
	if (battery) {
	// turn on BBSQW flag
	temp_buffer = temp_buffer | 0x40;
	} else {
	// turn off BBSQW flag
	temp_buffer = temp_buffer & 0xBF;
	}
	if (TF) {
	// set ~EOSC to 0 and INTCN to zero.
	temp_buffer = temp_buffer & 0x7B;
	} else {
	// set ~EOSC to 1, leave INTCN as is.
	temp_buffer = temp_buffer | 0x80;
	}
	// shift frequency into bits 3 and 4 and set.
	frequency = frequency << 3;
	temp_buffer = temp_buffer | frequency;
	// And write the control bits
	writeControlByte(temp_buffer, 0);
	}
	// Enable 32kHz
	void DS3231::enable32kHz(bool TF) {
	// turn 32kHz pin on or off
	byte temp_buffer = readControlByte(1);
	if (TF) {
	// turn on 32kHz pin
	temp_buffer = temp_buffer | 0x08;
	} else {
	// turn off 32kHz pin
	temp_buffer = temp_buffer & 0xF7;
	}
	writeControlByte(temp_buffer, 1);
	}
	// Oscillator Check
	bool DS3231::oscillatorCheck() {
	// Returns false if the oscillator has been off for some reason.
	// If this is the case, the time is probably not correct.
	byte temp_buffer = readControlByte(1);
	bool result = true;
	if (temp_buffer & 0x80) {
	// Oscillator Stop Flag (OSF) is set, so return false.
	result = false;
	}
	return result;
	}
	
	bool DS3231::Read(){
	
	Wire->beginTransmission(DS3231_ADDRESS);
	Wire->write(DS3231_SECOND_REGISTER); //registro inicial
	uint8_t ret = Wire->endTransmission();
	
	if (ret!=0)
	return false; //A failure has occur
	
	Wire->requestFrom(DS3231_ADDRESS, 7);
	
	ss = bcd2bin(Wire->read());
	mm = bcd2bin(Wire->read());
	hh = bcd2bin(Wire->read());
	Wire->read(); //dia da semana
	d = bcd2bin(Wire->read());
	m = bcd2bin(Wire->read()& 0x1F);
	y = bcd2bin(Wire->read());
	
	return true;
	
	}
	
	
	bool DS3231::Read_Temperature(){
	
	Wire->beginTransmission(DS3231_ADDRESS);
	Wire->write(DS3231_TEMPERATURE_REGISTER_HIGH_BYTE); //registro inicial
	Wire->endTransmission();
	
	Wire->requestFrom(DS3231_ADDRESS, 2); //Quantidade de Bytes
	
	Temp = Wire->read();
	Temp_float = Wire->read();
	
	
	
	return true;
	
	}
	
	double DS3231::Get_Temperature()
	{
	return (Temp + ((Temp_float>>6) * 0.25));
	}
	
	bool DS3231::Force_Temperature_Update()
	{
	
	Wire->beginTransmission(DS3231_ADDRESS);
	Wire->write(DS3231_CONTROL);
	Wire->write(CONV_TEMPERATURE_FORCE_BIT);
	Wire->endTransmission();
	
	Wire->requestFrom(DS3231_ADDRESS, 1);
	
	while (Wire->read() & CONV_TEMPERATURE_FORCE_BUSY_BIT);
	
	return true;
	}
	
	bool DS3231::Adjust_Time(const DateTime& dt) {
	Wire->beginTransmission(DS3231_ADDRESS);
	Wire->write(DS3231_SECOND_REGISTER);
	Wire->write(bin2bcd(dt.second()));
	Wire->write(bin2bcd(dt.minute()));
	Wire->write(bin2bcd(dt.hour()));
	Wire->write(bin2bcd(0));
	Wire->write(bin2bcd(dt.day()));
	Wire->write(bin2bcd(dt.month()));
	Wire->write(bin2bcd(dt.year() - 2000));
	Wire->endTransmission();
	
	return true;
	//
	//  uint8_t statreg = read_i2c_register(DS3231_ADDRESS, DS3231_STATUSREG);
	//  statreg &= ~0x80; // flip OSF bit
	//  write_i2c_register(DS3231_ADDRESS, DS3231_STATUSREG, statreg);
	}
	/*
	* DS3231.cpp
	*
	*  Created on: 18 de jan de 2019
	*      Author: educampos
	*/
	
	#include <DateTime.h>
	#include <DS3231.h>
	
	
	static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
	static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }
	
	
	DS3231::DS3231() {
	
	}
	
	DS3231::~DS3231() {
	
	}
	
	bool DS3231::Initialize(TwoWire* Wire)
	{
	this->Wire = Wire;
	
	return true;
	}
	
	void writeControlByte(byte control, bool which){ // Define Control bits
	//Write the selected control byte.
	// which=false -> 0x0e, true -> 0x0f.
	
	Wire.beginTransmission(DS3231_ADDRESS);
	if(which){
	Wire.write(0x0f);
	}else{
	Wire.write(0x0e);
	}
	Wire.write(control);
	Wire.endTransmission();
	}
	
	byte readControlByte(bool which) {
	// Read selected control byte
	// first byte (0) is 0x0e, second (1) is 0x0f
	Wire.beginTransmission(DS3231_ADDRESS);
	if (which) {
	// second control byte
	Wire.write(0x0f);
	} else {
	// first control byte
	Wire.write(0x0e);
	}
	Wire.endTransmission();
	Wire.requestFrom(DS3231_ADDRESS, 1);
	return Wire.read();
	}
	
	
	// enable oscillator
	void enableOscillator(bool TF, bool battery, byte frequency){
	// turns oscillator on or off. True is on, false is off.
	// if battery is true, turns on even for battery-only operation,
	// otherwise turns off if Vcc is off.
	// frequency must be 0, 1, 2, or 3.
	// 0 = 1 Hz
	// 1 = 1.024 kHz
	// 2 = 4.096 kHz
	// 3 = 8.192 kHz (Default if frequency byte is out of range)
	if (frequency > 3) frequency = 3;
	// read control byte in, but zero out current state of RS2 and RS1.
	byte temp_buffer = readControlByte(0) & 0xE7;
	if (battery) {
	// turn on BBSQW flag
	temp_buffer = temp_buffer | 0x40;
	} else {
	// turn off BBSQW flag
	temp_buffer = temp_buffer & 0xBF;
	}
	if (TF) {
	// set ~EOSC to 0 and INTCN to zero.
	temp_buffer = temp_buffer & 0x7B;
	} else {
	// set ~EOSC to 1, leave INTCN as is.
	temp_buffer = temp_buffer | 0x80;
	}
	// shift frequency into bits 3 and 4 and set.
	frequency = frequency << 3;
	temp_buffer = temp_buffer | frequency;
	// And write the control bits
	writeControlByte(temp_buffer, 0);
	}
	// Enable 32kHz
	void DS3231::enable32kHz(bool TF) {
	// turn 32kHz pin on or off
	byte temp_buffer = readControlByte(1);
	if (TF) {
	// turn on 32kHz pin
	temp_buffer = temp_buffer | 0x08;
	} else {
	// turn off 32kHz pin
	temp_buffer = temp_buffer & 0xF7;
	}
	writeControlByte(temp_buffer, 1);
	}
	// Oscillator Check
	bool DS3231::oscillatorCheck() {
	// Returns false if the oscillator has been off for some reason.
	// If this is the case, the time is probably not correct.
	byte temp_buffer = readControlByte(1);
	bool result = true;
	if (temp_buffer & 0x80) {
	// Oscillator Stop Flag (OSF) is set, so return false.
	result = false;
	}
	return result;
	}
	
	bool DS3231::Read(){
	
	Wire->beginTransmission(DS3231_ADDRESS);
	Wire->write(DS3231_SECOND_REGISTER); //registro inicial
	uint8_t ret = Wire->endTransmission();
	
	if (ret!=0)
	return false; //A failure has occur
	
	Wire->requestFrom(DS3231_ADDRESS, 7);
	
	ss = bcd2bin(Wire->read());
	mm = bcd2bin(Wire->read());
	hh = bcd2bin(Wire->read());
	Wire->read(); //dia da semana
	d = bcd2bin(Wire->read());
	m = bcd2bin(Wire->read()& 0x1F);
	y = bcd2bin(Wire->read());
	
	return true;
	
	}
	
	
	bool DS3231::Read_Temperature(){
	
	Wire->beginTransmission(DS3231_ADDRESS);
	Wire->write(DS3231_TEMPERATURE_REGISTER_HIGH_BYTE); //registro inicial
	Wire->endTransmission();
	
	Wire->requestFrom(DS3231_ADDRESS, 2); //Quantidade de Bytes
	
	Temp = Wire->read();
	Temp_float = Wire->read();
	
	
	
	return true;
	
	}
	
	double DS3231::Get_Temperature()
	{
	return (Temp + ((Temp_float>>6) * 0.25));
	}
	
	bool DS3231::Force_Temperature_Update()
	{
	
	Wire->beginTransmission(DS3231_ADDRESS);
	Wire->write(DS3231_CONTROL);
	Wire->write(CONV_TEMPERATURE_FORCE_BIT);
	Wire->endTransmission();
	
	Wire->requestFrom(DS3231_ADDRESS, 1);
	
	while (Wire->read() & CONV_TEMPERATURE_FORCE_BUSY_BIT);
	
	return true;
	}
	
	bool DS3231::Adjust_Time(const DateTime& dt) {
	Wire->beginTransmission(DS3231_ADDRESS);
	Wire->write(DS3231_SECOND_REGISTER);
	Wire->write(bin2bcd(dt.second()));
	Wire->write(bin2bcd(dt.minute()));
	Wire->write(bin2bcd(dt.hour()));
	Wire->write(bin2bcd(0));
	Wire->write(bin2bcd(dt.day()));
	Wire->write(bin2bcd(dt.month()));
	Wire->write(bin2bcd(dt.year() - 2000));
	Wire->endTransmission();
	
	return true;
	//
	//  uint8_t statreg = read_i2c_register(DS3231_ADDRESS, DS3231_STATUSREG);
	//  statreg &= ~0x80; // flip OSF bit
	//  write_i2c_register(DS3231_ADDRESS, DS3231_STATUSREG, statreg);
	}
