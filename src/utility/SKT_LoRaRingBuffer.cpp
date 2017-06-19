/*
 * Copyright (C) WIZnet, Inc. All rights reserved.
 * Use is subject to license terms.
 */
#include "SKT_LoRaRingBuffer.h"

SKT_LoRaRingBuffer::SKT_LoRaRingBuffer( int size )
{
    bufferSize = size+1;
    bufferTail = 0;
    bufferHead = 0;
    buffer_p = ( byte* )malloc( bufferSize );
        memset( buffer_p, 0, bufferSize );
}

SKT_LoRaRingBuffer::~SKT_LoRaRingBuffer()
{
    if( buffer_p )
          free( buffer_p );
}

// public functions
int SKT_LoRaRingBuffer::reset()
{
    bufferHead = 0;
    bufferTail = 0;

    memset(buffer_p, 0, bufferSize);
}

int SKT_LoRaRingBuffer::available(void)
{
  //int ByteCount = (bufferSize + bufferHead - bufferTail) % bufferSize;
  int ByteCount = (bufferSize + bufferHead - bufferTail);
  while( ByteCount >= bufferSize )
  {
    ByteCount = ByteCount - bufferSize;
  }
  return ByteCount;
}

bool SKT_LoRaRingBuffer::isFull()
{
    if( available() >= bufferSize)  return true;

    return false;
}

int SKT_LoRaRingBuffer::getBufferSize()
{
    return bufferSize;
}

int SKT_LoRaRingBuffer::read(void)
{
  if (bufferHead == bufferTail) {
    return -1;
  }
  else {
    byte c = buffer_p[bufferTail];
    bufferTail = (bufferTail + 1) % bufferSize;
    if(bufferHead == bufferTail) {
      bufferTail = 0;
      bufferHead = 0;
    }
    return c;
  }
}

byte SKT_LoRaRingBuffer::write( int c )
{
  //if ((bufferHead + 1) % bufferSize == bufferTail) {
  //  return -1;
  //}
  buffer_p[bufferHead] = c;
  bufferHead = (bufferHead + 1) % bufferSize;
  return 0;
}

void SKT_LoRaRingBuffer::remove(int n)
{
  if(bufferHead != bufferTail) {
    bufferTail = (bufferTail + n) % bufferSize;
  }
}

int SKT_LoRaRingBuffer::peek(void)
{
  if (bufferHead == bufferTail) {
    return -1;
  }
  else {
    return buffer_p[bufferTail];
  }
}

int SKT_LoRaRingBuffer::peek(int n) {

  if (bufferHead == bufferTail) {
    return -1;
  }
  else {
    return buffer_p[(bufferTail + n) % bufferSize];
  }
}

void SKT_LoRaRingBuffer::init()
{
    reset();
}

void SKT_LoRaRingBuffer::push(char c)
{
    write(c);
}

void SKT_LoRaRingBuffer::push_string(const char* str)
{
    int str_len = strlen(str);
    for(int i=0; i<str_len; i++)
    {
        write(str[i]);
    }
}

bool SKT_LoRaRingBuffer::endsWith(const char* str)
{
    int buffer_len;

    int findStrLen = strlen(str);

    char *p1 = (char*)&str[0];
    char *p2 = p1 + findStrLen;

    unsigned int tail = bufferHead - findStrLen;

    for(char *p=p1; p<p2; p++)
    {
        if(*p != buffer_p[tail])
        {
            return false;
        }

        tail++;

        if( tail == bufferSize)     tail = 0;
    }

    return true;
}

char* SKT_LoRaRingBuffer::FindStr(const char* str)
{
    char *p_find_start = (char*)&str[0];
    char *p_src_start  = (char*)&buffer_p[bufferTail];

    return strstr((char*)p_src_start,(char*)p_find_start);
}


int SKT_LoRaRingBuffer::getString(char* dest, int size)
{
    char *p = (char*)&buffer_p[bufferTail];
    strncpy((char*)dest,(char*)p,size);
    return 0;
}

void SKT_LoRaRingBuffer::getStrN(char *dest, unsigned int skipChars, unsigned int num)
{
    int len = ( bufferSize + bufferHead - bufferTail) % bufferSize - skipChars;

    if (len > num)
        len = num;

    char *p = (char*)&buffer_p[bufferTail];
    strncpy(dest,(char*)p,len);
    dest[len]=0;
}


int SKT_LoRaRingBuffer::getLine(char* dest, int size, char sep, int skipchar)
{
    int i,len=0,byteCnt=0;
    char ch;


    byteCnt = available();

    for(i=0; i<byteCnt; i++)
    {
        ch = (char)read();
        if(ch == sep)       break;

        if(i > size)
        {
            continue;
        }

        dest[i] = ch;
    }
    len = i;


    for(i=0;i<skipchar;i++) read();

    return len;
}

void SKT_LoRaRingBuffer::printbuffer()
{
    char *p = (char*)&buffer_p[bufferTail];
#if defined(ARDUINO_ARCH_SAMD) 
    SerialUSB.println(p);
#else
    Serial.println(p);
#endif
}

char* SKT_LoRaRingBuffer::getStr()
{
    char *p = (char*)&buffer_p[bufferHead];
    *p = 0;

    return (char*)&buffer_p[bufferTail];

}
