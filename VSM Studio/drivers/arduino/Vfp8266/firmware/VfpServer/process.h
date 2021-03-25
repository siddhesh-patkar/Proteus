/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  VFP Server for ESP8266                  *****/
/*****                                                          *****/
/*****                 Pseudo Process Base Class                *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#pragma once

#include <arduino.h>

class Process :  public Stream
 { public:
      Process() { returnCode = -1; }
      virtual ~Process() {}
      
      // Overloadables
      virtual void run() {}
      virtual bool isRunning () { return false; }
      virtual int  wait() { return returnCode; }
      virtual int availableOutput();
      virtual String readOutput(int maxlen);
      virtual void writeInput(const String &data);

      // Stream implementation
      virtual int available();
      virtual int read();
      virtual int peek();
      virtual void flush() {}

      // Print implementation
      virtual size_t write(uint8_t);
      virtual size_t write(const uint8_t *buffer, size_t size);

      // Extra
      bool canReadLine() { return inputBuffer.indexOf('\n') != -1; }
         
   protected:   
      int returnCode;
      class Buffer : public String
       { public:
            Buffer &operator=(const String &s) { String::operator=(s); return *this; }
            void concat (char c) { String::concat(c); }
            void concat (const uint8_t *buffer, size_t size) { String::concat((const char *)buffer, size); }
       } inputBuffer, outputBuffer;            
 }; 
 

