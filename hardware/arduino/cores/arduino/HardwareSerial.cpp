/*
  HardwareSerial.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
// #include "Arduino.h"
#include "wiring_private.h"

#include "HardwareSerial.h"

// this next line disables the entire HardwareSerial.cpp, 
// this is so I can support Attiny series and any other chip without a uart
#if defined(UBRRH) || defined(UBRR0H) || defined(UBRR1H) || defined(UBRR2H) || defined(UBRR3H)

#if defined(USBCON)
  ring_buffer rx_buffer = { { 0 }, 0, 0};
#endif
#if defined(UBRRH) || defined(UBRR0H)
  ring_buffer rx_buffer  =  { { 0 }, 0, 0 };
#endif
#if defined(UBRR1H)
  ring_buffer rx_buffer1  =  { { 0 }, 0, 0 };
#endif
#if defined(UBRR2H)
  ring_buffer rx_buffer2  =  { { 0 }, 0, 0 };
#endif
#if defined(UBRR3H)
  ring_buffer rx_buffer3  =  { { 0 }, 0, 0 };
#endif

// Preinstantiate Objects //////////////////////////////////////////////////////

#if defined(UBRRH) && defined(UBRRL)
  HardwareSerial Serial(&rx_buffer, 0);
#elif defined(UBRR0H) && defined(UBRR0L)
  HardwareSerial Serial(&rx_buffer, 0);
#elif defined(USBCON)
  // do nothing - Serial object and buffers are initialized in CDC code
#else
  #error no serial port defined  (port 0)
#endif

#if defined(UBRR1H)
  HardwareSerial Serial1(&rx_buffer1, 1);
#endif
#if defined(UBRR2H)
  HardwareSerial Serial2(&rx_buffer2, 2);
#endif
#if defined(UBRR3H)
  HardwareSerial Serial3(&rx_buffer3, 3);
#endif

void serialEvent() __attribute__((weak));
void serialEvent() {}
void serialEvent1() __attribute__((weak));
void serialEvent1() {}
void serialEvent2() __attribute__((weak));
void serialEvent2() {}
void serialEvent3() __attribute__((weak));
void serialEvent3() {}

void serialEventRun(void)
{
#if defined(UBRRH)
  if (Serial.available()) serialEvent();
#endif
#if defined(UBRR1H)
  if (Serial1.available()) serialEvent1();
#endif
#if defined(UBRR2H)
  if (Serial2.available()) serialEvent2();
#endif
#if defined(UBRR3H)
  if (Serial3.available()) serialEvent3();
#endif
}

// Constructors ////////////////////////////////////////////////////////////////

HardwareSerial::HardwareSerial(ring_buffer *rx_buffer, int serial_nr)
{
  _rx_buffer = rx_buffer;
  _serial_nr = serial_nr;
}

// Public Methods //////////////////////////////////////////////////////////////

void HardwareSerial::begin(unsigned long baud)
{
}

void HardwareSerial::begin(unsigned long baud, byte config)
{
}

void HardwareSerial::end()
{  
  // clear any received data
  _rx_buffer->head = _rx_buffer->tail;
}

void HardwareSerial::flush()
{
}

size_t HardwareSerial::write(uint8_t c)
{
  long data[5] = {wh, 42, _serial_nr, c, globalDelay};
  emscripten_worker_respond((char*) data, sizeof(data));
  
  return 1;
}

HardwareSerial::operator bool() {
	return true;
}

void HardwareSerial::store_char(unsigned char c)
{
  int i = (unsigned int)(_rx_buffer->head + 1) % SERIAL_BUFFER_SIZE;

  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if (i != _rx_buffer->tail) {
    _rx_buffer->buffer[_rx_buffer->head] = c;
    _rx_buffer->head = i;
  }
}

#endif // whole file

