
// incluir bibliotecas
#include "Arduino.h"
#include "LM35.h"


// definir pinos
#define LM35_PIN_VOUT	A3



// variaveis Globais e definicoes

// inicializacao de objeto
LM35 lm35(LM35_PIN_VOUT);



// define variaveis para menu teste
const int timeout = 10000;       //define timeout de 10 sec
char menuOption = 0;
long time0;

// Setup essencial para o circuito funcionar (roda toda vez que conectado na alimentacao)
void setup() 
{
    // Setup Serial (util para debbug)
    // Serial Monitor para ver mensagens de print
    Serial.begin(9600);
    while (!Serial) ; // esperar a serial port conectar
    Serial.println("start");
    
    
    menuOption = menu();
    
}

// Main
void loop() 
{
    
    
    if(menuOption == '1') {
    // LM35DZ - Sensor de Temperatura - Teste
    // pega medida do sensor
    float lm35TempC = lm35.getTempC();
    Serial.print(F("Temp: ")); Serial.print(lm35TempC); Serial.println(F("[°C]"));

    }
    
    
    
    if (millis() - time0 > timeout)
    {
        menuOption = menu();
    }
    
}



// funcao do menu para selecionar componentes a serem testados
// instrucoes do menu serial
char menu()
{

    Serial.println(F("\nQual componente gostaria de testar?"));
    Serial.println(F("(1) LM35"));
    Serial.println(F("(menu) aperte botao reset na placa\n"));
    while (!Serial.available());

    // Read data from serial monitor if received
    while (Serial.available()) 
    {
        char c = Serial.read();
        if (isAlphaNumeric(c)) 
        {
            if(c == '1') 
    			Serial.println(F("Testando LM35"));
            else
            {
                Serial.println(F("input invalido"));
                return 0;
            }
            time0 = millis();
            return c;
        }
    }
}
