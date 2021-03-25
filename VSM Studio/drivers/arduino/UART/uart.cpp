/* UART object */

#include "uart.h"


String UART::input (bool echo)
 { String s;
   int c; 
   do 
    { c = recv();
      if (c == -1)
         continue;
      if (c == 0x08)
       { if (s.length() > 0)
          { s = s.substring(0, s.length()-1);
            if (echo) send(c);
          }
       }
      else if (c >= 0x20)
       { s += char(c);
         if (echo) send(c);
       }
    } while (c != 0x0D);
   if (echo)
      Serial.println();   
   return s; 
 }


