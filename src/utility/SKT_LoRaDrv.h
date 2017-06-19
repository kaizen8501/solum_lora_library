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

#ifndef SKT_LoRaDrv_h 
#define SKT_LoRaDrv_h

#include "Stream.h"
#include "IPAddress.h"
#include "SKT_LoRaRingBuffer.h"
#include "SKT_LoRa_definitions.h"
#include "debug.h"

#define JOIN_TIMEOUT 3000000 //5Min
#define DATA_SEND_TIMEOUT 65000 

class SKT_LoRaDrv
{
public:
    SKT_LoRaDrv ();
    static int16_t _state;

private:
    static void   LoRaDriverInit   (Stream *skt_loraSerial);
    static bool   LoRaResetJoin            ();
    
    static char*   getFwVersion        ();


    static Stream *SKT_LoRaSerial;

    static long _bufPos;

    static SKT_LoRaRingBuffer ringBuf;
    static SKT_LoRaRingBuffer dl_data_ringBuf;
    static char     fwVersion[FW_VER_LENGTH];

public:
    static uint16_t availData   ();
    static int  getData         ();
    static int  getDataBuf      (uint8_t* buf, uint16_t bufSize);

    static int  getResponse     (char* outStr, int outStrLen, int lineNum);
    static int  sendCmd         (const char* cmd, int timeout=1000, ...);
    static int  SendCmdWithTag  (const char* cmd, const char* tag, const char* tag2, int timeout);
    
    static int  readUntil     (int timeout, const char* tag="[OK]\r\n", const char* tag2="", const char* error="[ERROR]\r\n");
    static void SKT_LoRaEmptyBuf (bool warn=true);
    static bool LoRaSendData(const char* data);

private:
    
    friend class SKT_LoRaClass;
    
};

extern SKT_LoRaDrv skt_loraDrv;

#endif 
