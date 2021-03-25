// Wrapper for the main serial port

#ifndef __UART_H__
#define __UART_H__

#pragma once

#include <core.h>
#include <HardwareSerial.h>

class UART
 { public:
      UART (HardwareSerial &s) : serial(s) { setDelimiter(' ');}
      void setup (uint16_t baud, uint8_t config) { serial.begin(baud, config); }
      void setDelimiter(char d) { readList.setDelimiter(d); writeList.setDelimiter(d); }
      void setPlaces (uint8_t p) { printList.setPlaces(p); }
      void setBase(uint8_t b) { printList.setBase(b); }
      String input(bool echo);
      StreamList &read() { return readList.begin(&serial);  }
      PrintList &write() { return writeList.begin(false, &serial); }
      PrintList &print() { return printList.begin(false, &serial); }
      PrintList &println() { return printList.begin(true, &serial); }
      bool operator()() { return serial.available(); }
      void send (uint8_t c) { serial.print(char(c)); }
      int recv() { return serial.read(); }
      
   private:
      HardwareSerial &serial;
      StreamList readList;
      PrintList writeList, printList;
 };       

#ifdef HAVE_HWSERIAL0
extern UART Uart;
#endif

#ifdef HAVE_HWSERIAL1
extern UART Uart1;
#endif

#ifdef HAVE_HWSERIAL2
extern UART Uart2;
#endif

#ifdef HAVE_HWSERIAL3
extern UART Uart3;
#endif

#endif