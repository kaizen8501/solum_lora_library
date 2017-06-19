/**
 ******************************************************************************
 * @project  UART Serial Hello world
 * @author   Edward Ahn, Jim Jun
 * @version  V1.0.1
 * @date     26-MAY-2016
 * @brief    Hello world
*******************************************************************************
**/

#include "SKT_LoRa.h"
#include "TinyGPS_Plus.h"

char test_data = 0;
char send_buf[100];
char lat_buf[100];
char lng_buf[100];


float latitude;
float longitude; 

TinyGPSPlus gps;

void setup() {
  SerialUSB.begin(9600); // Serial initial Debug
  Serial2.begin(9600);   // Begin Serial2 GPS
  
  SerialUSB.print("Setup Done");
  Serial2.print("Serial2 Test");


  Serial5.begin(38400); // Begin Serial_5 LoRa
  LoRa.init(&Serial5);
  LoRa.begin();
}

void loop() {
  while((Serial2.available())>0)
  {
    test_data = Serial2.read();
    //SerialUSB.write(test_data);
    gps.encode(test_data);
  }
  sensing_gps();
}

void sensing_gps()
{
  int i;
  
  if (gps.location.isValid())
  {
    latitude = (float)(gps.location.lat());
    longitude = (float)(gps.location.lng());
    dtostrf(latitude, 6, 6, lat_buf);
    for (i=0; i<strlen(lat_buf); i++)
    {
      if (lat_buf[i] == '.')
      {
        lat_buf[i] = 0xAA;
        break;
      }
    }
    SerialUSB.print(F(lat_buf));
    SerialUSB.print("  ");
  
    dtostrf(longitude, 6, 6, lng_buf);
    for (i=0; i<strlen(lng_buf); i++)
    {
      if (lng_buf[i] == '.')
      {
        lng_buf[i] = 0xAA;
        break;
      }
    }
    SerialUSB.print(F(lng_buf));
    SerialUSB.print("\r\n");
    
    sprintf(send_buf, "FF%sBB%sFE", lat_buf, lng_buf);
    SerialUSB.print(F(send_buf));
    LoRa.Send(send_buf, 3);
    delay(60000);
  }
}

char *dtostrf(double value, int width, unsigned int precision, char *result)
{
  int decpt, sign, reqd, pad;
  const char *s, *e;
  char *p;
  s = fcvt(value, precision, &decpt, &sign);
  if (precision == 0 && decpt == 0) {
  s = (*s < '5') ? "0" : "1";
    reqd = 1;
  } else {
    reqd = strlen(s);
    if (reqd > decpt) reqd++;
    if (decpt == 0) reqd++;
  }
  if (sign) reqd++;
  p = result;
  e = p + reqd;
  pad = width - reqd;
  if (pad > 0) {
    e += pad;
    while (pad-- > 0) *p++ = ' ';
  }
  if (sign) *p++ = '-';
  if (decpt <= 0 && precision > 0) {
    *p++ = '0';
    *p++ = '.';
    e++;
    while ( decpt < 0 ) {
      decpt++;
      *p++ = '0';
    }
  }    
  while (p < e) {
    *p++ = *s++;
    if (p == e) break;
    if (--decpt == 0) *p++ = '.';
  }
  if (width < 0) {
    pad = (reqd + width) * -1;
    while (pad-- > 0) *p++ = ' ';
  }
  *p = 0;
  return result;
}

