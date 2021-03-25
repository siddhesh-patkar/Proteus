
#include "spiport.h"

SpiPort Spi;

void SpiPort::beginTransaction (uint8_t pin)
 { if (pin != ss)
    { pinMode(ss=pin, OUTPUT);      
      digitalWrite(ss, HIGH);      
    }
   digitalWrite(ss, LOW); 
   SPI.beginTransaction(settings);      
 }

void SpiPort::endTransaction()
 { SPI.endTransaction();
   digitalWrite(ss, HIGH);
 }

SpiList &SpiList::begin(uint8_t w)
 { wide=w; return *this; 
 }
 
SpiList &SpiList::arg(int16_t i)
 { if (wide)  
      SPI.transfer16(i); 
   else 
      SPI.transfer(i); 
   return *this; 
 }
  
SpiList &SpiList::arg(const char *s) 
 { while (*s) 
    { if (wide)  
         SPI.transfer16(*s); 
      else 
         SPI.transfer(*s);
      s++;    
    }
   return *this;  
 }

SpiList &SpiList::arg(const String &s)
 { for (unsigned i=0; i<s.length(); ++i) 
      if (wide)  
         SPI.transfer16(s[i]); 
      else 
         SPI.transfer(s[i]); 
   return *this; 
 }


