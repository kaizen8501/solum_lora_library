/*
 * Copyright (C) WIZnet, Inc. All rights reserved.
 * Use is subject to license terms.
 */
#ifndef WIZFI250_LIGHT_UTILITY_RINGBUFFER_H_
#define WIZFI250_LIGHT_UTILITY_RINGBUFFER_H_


#include "Arduino.h"

class SKT_LoRaRingBuffer
{
private:

    int bufferSize;
    unsigned int bufferHead, bufferTail;
    byte *buffer_p;

public:
    SKT_LoRaRingBuffer(int size);
        ~SKT_LoRaRingBuffer();
    int reset();
    int available(void);
    bool isFull();
    int getBufferSize();
    int peek(void);
    int peek(int);
    void remove(int);
    int read(void);
    byte write(int);

    void    init();
    void    push(char c);
    void    push_string(const char* str);

    bool    endsWith    (const char* str);
    char*   FindStr     (const char* str);
    int     getString   (char* dest, int size);
    void    getStrN     (char *dest, unsigned int skipChars, unsigned int num);
    int     getLine     (char* dest, int size, char sep, int skipchar);
    char*   getStr      (void);
    

    void    printbuffer ();
};

#endif /* WIZFI250_LIGHT_UTILITY_RINGBUFFER_H_ */
