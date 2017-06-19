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

#include <Arduino.h>

#ifndef __arm__
#include <avr/pgmspace.h>
#else
#define PROGMEM const
#define F(x) x
#endif

#include "debug.h"
#include "SKT_LoRaDrv.h"
#include "general_parse.h"
#include <stdarg.h>

// maximum size of AT command
#define CMD_BUFFER_SIZE 512

typedef enum
{
    TAG_ERROR = -1,
    TAG_OK = 0,
//  TAG_CONNECT,
//  TAG_DISCONNECT,
//    TAG_LINKUP,
//    TAG_LINKDOWN
    TAG_NUM
} TagsEnum;

Stream *SKT_LoRaDrv::SKT_LoRaSerial;
SKT_LoRaRingBuffer SKT_LoRaDrv::ringBuf(CMD_BUFFER_SIZE);
SKT_LoRaRingBuffer SKT_LoRaDrv::dl_data_ringBuf(CMD_BUFFER_SIZE);

SKT_LoRaDrv::SKT_LoRaDrv()
{
}

void SKT_LoRaDrv::LoRaDriverInit(Stream *skt_loraSerial)
{
    LOGDEBUG(F("> LoRaDriverInit"));
    SKT_LoRaDrv::SKT_LoRaSerial = skt_loraSerial;
}



bool SKT_LoRaDrv::LoRaResetJoin()
{
	LOGDEBUG(F("> LoRaResetJoin"));
	if( SendCmdWithTag("AT+RST\r\n", "JOINED", NULL, JOIN_TIMEOUT) == TAG_ERROR)
	{
		LOGDEBUG(F("> LoRaResetJoin Fail"));
		delay(3000);
		//NVIC_SystemReset();
	}
	return true;
}

bool SKT_LoRaDrv::LoRaSendData(const char* data)
{
	char dataBuf[CMD_BUFFER_SIZE];

	LOGDEBUG(F("> LoRaSendData"));
	sprintf(dataBuf, "AT+SEND 01%s\r\n", data);
	if( SendCmdWithTag(dataBuf, "RX DONE", NULL, DATA_SEND_TIMEOUT) == TAG_ERROR)
	{
		LOGDEBUG(F("> LoRaSendData Fail"));
		return false;
	}
	return true;
}

char* SKT_LoRaDrv::getFwVersion()
{
    //sendCmd(F("AT+MINFO\r"));
    //sendCmd("AT+MINFO\r");
    //getResponse(fwVersion, sizeof(fwVersion), 2);

    return fwVersion;
}


////////////////////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////////////////////
uint16_t SKT_LoRaDrv::availData()
{
    String recved_bytes;
    char mgmt_cmd_byte;
    unsigned long startMillis;
    int recv_cnt = 0;

    startMillis = millis();
    do {
        //recved_byte = SKT_LoRaSerial->read();
        recved_bytes = SKT_LoRaSerial->readStringUntil('\n');
        if (recved_bytes.length() <= 0)
        {
            return dl_data_ringBuf.available();
        }
        else
        {
            //LOGDEBUG(recved_bytes);
            if( recved_bytes.indexOf("SKT_EXT_DEVMGMT") != -1)
            {
                do{
                    mgmt_cmd_byte = (char)SKT_LoRaSerial->read();
                    if( mgmt_cmd_byte > 0 )
                    {
                        //LOGDEBUG0(mgmt_cmd_byte);
                        dl_data_ringBuf.push(mgmt_cmd_byte);
                    }
                }while(mgmt_cmd_byte != '\n');
            }
            else if( recved_bytes.indexOf("SKT_DEV_RESET") != -1)
            {
                dl_data_ringBuf.push_string("RESET\r\n");
            }

            if (dl_data_ringBuf.isFull())
            {
                break;
            }
        }
    } while (millis() - startMillis < 2000);

    return dl_data_ringBuf.available();
}

int SKT_LoRaDrv::getData()
{
    return dl_data_ringBuf.read();
}

int SKT_LoRaDrv::getDataBuf(uint8_t* buf, uint16_t bufSize)
{
    int i,idx=0;
    int recv_data=0;

    for(i=0; i<bufSize; i++)
    {
        recv_data = dl_data_ringBuf.read();
        if( recv_data > 0 )
        {
            buf[idx] = (uint8_t)recv_data;
            idx++;
        }
        else
        {
            break;
        }
    }
    buf[idx] = '\0';
    return idx;
}

int SKT_LoRaDrv::getResponse(char* outStr, int outStrLen, int lineNum)
{
    int i,ret=0;

    for(i=0;i<lineNum;i++)
    {
        memset(outStr,0,outStrLen);
        ret = ringBuf.getLine(outStr,outStrLen,'\r',1);
    }

    return ret;
}

int SKT_LoRaDrv::sendCmd(const char* cmd, int timeout, ...)
{
    char cmdBuf[CMD_BUFFER_SIZE];

    va_list args;
    va_start (args, timeout);

#ifdef __AVR__
    vsnprintf_P (cmdBuf, CMD_BUFFER_SIZE, (char*)cmd, args);
#else
    vsnprintf (cmdBuf, CMD_BUFFER_SIZE, (char*)cmd, args);
#endif
    va_end(args);

    SKT_LoRaEmptyBuf();
    ringBuf.reset();

    LOGDEBUG(F("----------------------------------------------"));
    LOGDEBUG1(F(">>"), cmdBuf);

    SKT_LoRaSerial->print(cmdBuf);
    int idx = readUntil(timeout);

    LOGDEBUG1(F("---------------------------------------------- >"), idx);
    LOGDEBUG();

    return idx;
}

int SKT_LoRaDrv::SendCmdWithTag(const char* cmd, const char* tag, const char* tag2, int timeout)
{
    SKT_LoRaEmptyBuf(true);
    ringBuf.reset();

    LOGDEBUG(F("----------------------------------------------"));
    LOGDEBUG1(F(">>"), cmd);

    SKT_LoRaSerial->print(cmd);
    int idx = readUntil(timeout, tag, tag2);

    LOGDEBUG1(F("---------------------------------------------- >"), idx);
    LOGDEBUG();

    return idx;
}


int SKT_LoRaDrv::readUntil(int timeout, const char* tag, const char* tag2, const char* error)
{
    int ret = TAG_ERROR;
    unsigned long start = millis();
    uint8_t  recved_byte, is_found1=0, is_found2=0;

//    if( m_esc_state != ESC_IDLE )
//    {
//        LOGDEBUG2("readUnitl","m_esc_state",m_esc_state);
//        return TAG_ERROR;
//    }

	if( tag == NULL)
		is_found1 = 1;
	if( tag2 == NULL)
		is_found2 = 1;

    while( (millis() - start < (unsigned long)timeout) and ret < 0 )
    {
        if( SKT_LoRaSerial->available() )
        {
            recved_byte = (char)SKT_LoRaSerial->read();
            ringBuf.push(recved_byte);
            LOGDEBUG0((char)recved_byte);
        }
        else
        {
            if( is_found1 == 0 && ringBuf.FindStr(tag))
            {
                is_found1 = 1;
            }
            if( is_found2 == 0 && ringBuf.FindStr(tag2))
            {
                is_found2 = 1;
            }
            if( ringBuf.FindStr(error) )
            {
                return TAG_ERROR;
            }
        }

        if ( is_found1 && is_found2 )
        {
            ret = TAG_OK;
            break;
        }
    }
    if (millis() - start >= (unsigned long)timeout)
    {
        LOGWARN(F(">>> TIMEOUT >>>"));
    }

    return ret;
}

void SKT_LoRaDrv::SKT_LoRaEmptyBuf(bool warn)
{
    char c;
    int i=0;
    while(SKT_LoRaSerial->available() > 0)
    {
        c = SKT_LoRaSerial->read();
        if (i>0 and warn == true)
        {
            LOGDEBUG0(c);
        }
        i++;
    }
    if (i>0 and warn == true)
    {
        LOGDEBUG(F(""));
        LOGDEBUG1(F("Dirty characters in the serial buffer! >"), i);
    }
}

SKT_LoRaDrv skt_loraDrv;
