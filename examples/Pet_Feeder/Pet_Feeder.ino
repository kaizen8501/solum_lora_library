#include "Arduino.h"
#include <SKT_LoRa.h>
#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial LoRaSerial(10, 11); // RX, TX
Servo servo;
int servoPin = 9;

void setup()
{
    servo.attach(servoPin);
    servo.write(90);

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
        LoRa.read((uint8_t*)str,100);
        if( strcmp(str,"1000\r\n") == 0 )
        {
            Serial.println("Start Feed");
            servo.write(120);
            delay(5000);
            servo.write(90);
        }
    }
}

