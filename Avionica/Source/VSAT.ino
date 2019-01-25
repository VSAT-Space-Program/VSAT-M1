//Exemplo de como utilizar as classes criadas na interface do Arduino.
//Estou avaliando se será necessário compilar o programa final fora da interface Arduino 
// para evitar que muitos códigos e inicializações indesejadas sejam realizadas pela pataforma Arduino

//Uma opção é utilizar o Eclipse. Quem tiver interesse posso explicar como migrar de IDE e como fazer códigos mais limpos 


#include "stdlib.h"
#include <inttypes.h>


#include <avr/io.h>
#include <util/delay.h>
#include "Usart.h"

#include <Wire.h>
#include "DS3231.h"

DS3231 rtc;

void setup() {
  // put your setup code here, to run once:

  Wire.begin();
  rtc.Initialize(&Wire);


    // initialize serial communication
  USART.begin(57600);
  USART.write("Serial is ready");

}

void loop() {
  // put your main code here, to run repeatedly:


  _delay_ms(1000);

  rtc.Read();
  USART.write("\r\n Time: ");
  USART.write(rtc.d);
  USART.write("/");
  USART.write(rtc.m);
  USART.write("/");
  USART.write(rtc.y+2000);
  USART.write("  ");
  USART.write(rtc.hh);
  USART.write(":");
  USART.write(rtc.mm);
  USART.write(":");
  USART.write(rtc.ss);

  rtc.Read_Temperature();
  double Temp = rtc.Get_Temperature();
  USART.write("\r\n Temperature: ");
  USART.write(Temp,2);

  rtc.Force_Temperature_Update();

}
