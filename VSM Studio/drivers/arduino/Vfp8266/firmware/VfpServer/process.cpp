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

#include "bridge.h"
#include "process.h"

int Process::availableOutput () 
 { return outputBuffer.length(); 
 }

String Process::readOutput(int maxlen)
// Read up to maxlen chars from the output.
 { String result = outputBuffer.substring(0, maxlen);
   outputBuffer = outputBuffer.substring(result.length());
   return result;
 }    

void Process::writeInput(const String &data)
// Add data to the input buffer.
 { inputBuffer += data;  
 }


int Process::available ()
 { return inputBuffer.length();  
 }

int Process::read()
// Read one character of the input, or return -1.
 { int result = -1;
   if (inputBuffer.length() > 0)
    { result = inputBuffer[0];
      inputBuffer = inputBuffer.substring(1);
    }
   return result; 
 }
      
int Process::peek()
// Peek the input buffer.
 { return inputBuffer.length() > 0 ? inputBuffer[0] : -1;
 }

size_t Process::write (uint8_t c)
// Write one byte to the output buffer.
 { outputBuffer.concat(char(c));
 }    
 
size_t Process::write(const uint8_t *buffer, size_t size)
 // Write a block of bytes to the output buffer.
  { outputBuffer.concat(buffer, size); 
  }      

