/*--------------------------------------------------------------------
This file is part of the Arduino WiFiEsp library.

The Arduino WiFiEsp library is free software: you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

The Arduino WiFiEsp library is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with The Arduino WiFiEsp library.  If not, see
<http://www.gnu.org/licenses/>.
--------------------------------------------------------------------*/

#ifndef SKT_LoRa_h
#define SKT_LoRa_h

#include <Arduino.h>
#include <Stream.h>
#include <IPAddress.h>
#include <inttypes.h>

#include "utility/SKT_LoRaRingBuffer.h"
#include "utility/SKT_LoRaDrv.h"
#include "utility/debug.h"

class SKT_LoRaClass
{
public:
    SKT_LoRaClass();


    static void init(Stream *espSerial);
    static char* firmwareVersion();
    
    static int  begin      ();
    static int  Send       (const char *data, int retry);
    static int  available  ();
    static int  read       ();
    static int  read       (uint8_t *buf, size_t size);
 
private:
    static uint8_t wizfiMode;
};

extern SKT_LoRaClass LoRa;

#endif
