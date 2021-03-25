/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                 Visual Designer for Arduino              *****/
/*****                                                          *****/
/*****                       Print Buffer                       *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include <arduino.h>
#include <ctype.h>

#include "core.h"

PrintBuffer::PrintBuffer () {}
 
size_t PrintBuffer::write (uint8_t c)
 { s += (char)c;
   return 1;
 }
   
size_t PrintBuffer::write(const uint8_t *buffer, size_t size)
 { for (int i=0; i<size; ++i)
      s += (char)buffer[i];
   return size;   
 }

size_t PrintBuffer::printTo(Print &buffer) const
 { return buffer.print(s);
 }
