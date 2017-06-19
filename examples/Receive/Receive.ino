#include "Arduino.h"
#include <SKT_LoRa.h>
#include <SoftwareSerial.h>

SoftwareSerial LoRaSerial(10, 11); // RX, TX

void setup()
{
    Serial.begin(38400);
    LoRaSerial.begin(38400);
    LoRa.init(&LoRaSerial);
    LoRa.begin();
}

void loop()
{
    char str[100];
    if( LoRa.available() )
    {
        //Serial.print((char)LoRa.read());
        LoRa.read((uint8_t*)str,100);
        Serial.print(str);
    }
}

