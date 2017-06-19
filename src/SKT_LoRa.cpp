#include "SKT_LoRa.h"

SKT_LoRaClass::SKT_LoRaClass()
{
}

void SKT_LoRaClass::init(Stream *SKT_LoRaSerial)
{
    LOGINFO(F("Initializing SKT_LoRa module"));
    SKT_LoRaDrv::LoRaDriverInit(SKT_LoRaSerial);
}

char* SKT_LoRaClass::firmwareVersion()
{
    return SKT_LoRaDrv::getFwVersion();
}

int SKT_LoRaClass::begin()
{
    if ( SKT_LoRaDrv::LoRaResetJoin() )
        return RET_OK;
    return RET_OK;
}

int SKT_LoRaClass::Send(const char *data, int retry)
{
	while(retry--)
	{
		if ( SKT_LoRaDrv::LoRaSendData(data) )
        	return RET_OK;
	}
    return RET_FAIL;
}

int SKT_LoRaClass::available()
{
    int bytes = SKT_LoRaDrv::availData();
    if (bytes > 0)
    {
        return bytes;
    }
    return 0;
}

int SKT_LoRaClass::read()
{
    return SKT_LoRaDrv::getData();
}

int SKT_LoRaClass::read(uint8_t *buf, size_t size)
{
    return SKT_LoRaDrv::getDataBuf(buf, size);
}


SKT_LoRaClass LoRa;
