/*
 * core.cpp
 *
 *  Created on: 19 de Jan de 2019
 *      Author: Eduardo Lacerda Campos
 */


#include "stdlib.h"
#include <Wire.h>
#include "Usart.h"
#include "DS3231.h"
#include "SX1276.h"

/***************************
Pinout - Arduino 1
ADC6
ADC7
AREF
PB0 ---> Enable Sensors
PB1 ---> Enable Arduino 2
PB2 ---> Enable Camera
PB3
PB4
PB5 ---> Led
PB6 <--- XTAL1
PB7 <--- XTAL2
PC0
PC1
PC2
PC3
PC4 <--> ComunicaÃ§Ã£o I2C Data
PC5 ---> ComunicaÃ§Ã£o I2C CLK
PC6
PD0 ---> SX1276 Rx
PD1 <--- SX1276 Tx
PD2 ---> SX1276 M0 Lora
PD3 ---> SX1276 M1 Lora
PD4 <--- SX1276 AUX Lora
PD5
PD6
PD7


Pinout - Arduino 2
ADC6 <--- Leitura do LM35
ADC7
AREF
PB0 <--- AL422_DO0
PB1 <--- AL422_DO1
PB2 ---> SPI SS
PB3 ---> SPI MOSI
PB4 <--- SPI MISO
PB5 ---> Led + SPI SCK
PB6 <--- XTAL1
PB7 <--- XTAL2
PC0 ---
PC1 ---> AL422_RCLK
PC2 ---> AL422_RRST
PC3 ---> AL422_WEN
PC4 <--> ComunicaÃ§Ã£o I2C Data
PC5 ---> ComunicaÃ§Ã£o I2C CLK
PC6
PD0 ---> NEO6M Rx
PD1 <--- NEO6M Tx
PD2 <--- AL422_DO2
PD3 <--- AL422_DO3
PD4 <--- AL422_DO4
PD5 <--- AL422_DO5
PD6 <--- AL422_DO6
PD7 <--- AL422_DO7


****************************/

SX1276 LoRa(1);

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  DDRB=(1<<PB5);
  PORTB &= ~(1<<PB5);


  // initialize serial communication
  USART.begin(9600);
  USART.write("Serial is ready");

  LoRa.Initialize(&USART);

}


void loop() {
  char data[MAX_TX_SIZE];

  USART.write("\r\n Dados Recebidos: ");
  USART.write(LoRa.Received(data, MAX_TX_SIZE));
  
  _delay_ms(1000);
  
  // problema if (USART.available()){
  // le o byte recebido
  char received = USART.read();
  USART.write(received);

  // se o byte recebido corresponde ao caractere '1'(49) acende o LED
  if (received == '1'){
    digitalWrite(LED_BUILTIN, HIGH);
  }
  
  // se o byte recebido for diferente de '1'(49) apaga o LED
  if (received != '1'){
    digitalWrite(LED_BUILTIN, LOW);
  }
  
//  if Lora.Read(data,len)
//    {
//    if (data == Hand_shake)
//      Led 1 on Led off
//      Buzzer
//    if (data == PosiÃ§Ã£o)
//      Led 2 on ... off
//
//    }


}
